from flask import Flask, render_template, request, Response, session, abort

import db

app = Flask(__name__)

Message = (
    ('callsign', str, 8),
    ('frequency', int, 8),
    ('message', str, 40),
)

Frequency = (
    ('frequency', int, 8),
)

Messages = db.Store('messages.db', Message, 5)
Frequency = db.Store('freq.db', Frequency, 5)

Messages._memory_dump()


@app.route('/')
def hello_world():
    freq = Frequency.execute('select * from frequency limit 5 deck')
    return render_template('index.html', freq=freq)


@app.route('/ack/', methods=['POST', 'GET'])
def ack():
    last = Messages.execute(u"select * from messages")
    return Response(repr(last[-1]))


@app.route('/is_free_frequency/<frequency>', methods=['GET'])
def is_free_frequency(frequency):
    try:
        frequency = int(frequency)
    except:
        abort(500)
    frequency = Frequency.execute("select * from frequency where frequency = {0} limit 1".format(frequency))
    print(frequency)
    if frequency:
        return Response('False')
    return Response('True')


@app.route('/post/', methods=['POST'])
def post():
    id, fr, msg = request.form['callsign'], request.form['frequency'], request.form['message']
    id, fr, msg = id.decode('unicode-escape', errors='ignore'), fr, msg.encode('unicode-escape', errors='ignore')
    fr = int(fr)
    id = id.replace(u'\'', '')
    msg = msg.replace(u'\'', '')

    access = session.get('frequency', '')
    access = [x for x in access.split(',') if access]
    access = set(map(int, access))

    zzz = is_free_frequency(str(fr))
    is_frequency_free = zzz.get_data() == 'True'
    if not is_frequency_free and fr not in access:
        abort(501)

    try:
        added = Messages.execute(r"insert into messages values ('''{0}''', {1}, '''{2}''')".format(id, fr, msg))
    except KeyError:
        abort(502)

    last_frequency = Messages.execute(u"select * from messages limit 1 desc")['frequency']
    if last_frequency not in access:
        access.add(last_frequency)
        Frequency.execute(r'insert into frequency values ({0}, )'.format(last_frequency))

    session['frequency'] = ','.join(map(str, access))
    return render_template('post.html', added=bool(added), last_frequency=last_frequency)


@app.route('/dialog/<frequency>', methods=['GET'])
def dialog(frequency):
    access = session.get('frequency', '')
    access = [x for x in access.split(',') if access]
    access = set(map(int, access))
    try:
        frequency = int(frequency)
    except:
        abort(503)

    if frequency not in  access:
        abort(504)

    id = frequency
    all = Messages.execute(r"select * where frequency = {0} limit 5 DESC".format(id))
    return Response(repr(all))


if __name__ == '__main__':
    app.secret_key = 'A0Zr98j/3yX R~XHH!jmN]LWX/,?RT'  # change this
    app.debug = True  # o_O! TODO: fix this!
    app.run()#host='0.0.0.0')
