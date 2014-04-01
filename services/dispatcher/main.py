from flask import Flask, render_template, request, Response

import db

app = Flask(__name__)

Message = (
    ('callsign', str, 32),
    ('frequency', int, 8),
    ('message', str, 100),
)

Messages = db.Store('messages.db', Message, 5)


@app.route('/')
def hello_world():
    return render_template('index.html')


@app.route('/ack/', methods=['POST', 'GET'])
def ack():
    last = Messages.execute(u"select * from messages")
    print(last)
    return Response(repr(last[-1]))


@app.route('/post/', methods=['POST'])
def post():
    id, fr, msg = request.form['callsign'], request.form['frequency'], request.form['message']
    fr = int(fr)
    id = id.replace(u'\'', '')
    msg = msg.replace(u'\'', '')
    print(u"insert into messages values ('{0}', {1}, '{2}')".format(id, fr, msg))
    added = Messages.execute(u"insert into messages values ('{0}', {1}, '{2}')".format(id, fr, msg))
    Messages.sync()
    return render_template('post.html', added=bool(added))

if __name__ == '__main__':
    app.debug = True
    app.run()#host='0.0.0.0')
