from db import Store

__author__ = 'pahaz'

schema = (
    ('id', int, 3),
    ('v', str, 20),
)

import os

os.system('rm test.db')
s = Store('test.db', schema, 5)
for x in range(2):
    s.execute("insert values (2, 'privert')")
    s.execute("insert values (3, 'poka')")
    s.execute("insert values (4, 'hohoohoh')")

s._memory_dump()

z = s.execute('select where id > 2 limit 1')
assert z['id'] == 3, 'select limit'

z = s.execute('select where id > 2 limit 1 desk')
assert z['id'] == 4, 'select limit desk'

z = s.execute('select * where id = 2')
assert len(z) == 2, 'select * id = 2'

z = s.execute('delete where id > 2 limit 1')
z = s.execute('select where id > 2')
assert len(z) == 3, 'select after delete id > 2'

z = s.execute('delete * where id = 2')
z = s.execute('select where id = 2')
assert len(z) == 0, 'select after delete'

s.execute("insert values (2, 'HUHUHUHU!')")
z = s.execute('select where id = 2')
assert z['v'] == 'HUHUHUHU!', 'select after delete'

s._memory_dump()

s.delete()

s._memory_dump()

for x in range(20):
    s.execute("insert values (20, 'www{0}')".format(x))

z = s.execute('select * from frequency limit 5 deck')
print(z)
assert len(z) == 5, 'select limit!'

z = s.execute(u"select * from messages limit 1 desc")
assert z['id'] == 20, 'limit and desc'

# s._memory[0]['id'] = 22
# s._memory[0]['value'] = 'adawd'
s.sync()
