import re

__author__ = 'pahaz'
import parser


def _int_dump(val, len_):
    return ('{0:0' + str(len_) + '}').format(val) + '\0'


def _str_dump(val, len_):
    while len(val) < len_:
        val += '\0'
    return val + '\0'


DUMPERS = {
    int: _int_dump,
    str: _str_dump,
}


class Store(object):
    auto_sync = True
    auto_mem_resync_counter = 0

    @staticmethod
    def is_valid_schema(self):
        return all([len(x) == 3 and x[1] in DUMPERS for x in self])

    def __init__(self, db_name, schema, log_level=1):
        """
        schema example = (
            (name, type, len),
            ...
        )

        int
        str
        """
        self._log_lvl = log_level

        if not self.is_valid_schema(schema):
            self.error("Invalid DB schema")
            raise Exception('Schema is bead!')

        all = self.init_mem(db_name, schema)

        self._memory = all
        self._db_name = db_name
        self._schema = schema

    def info(self, msg):
        if self._log_lvl > 3:
            print(msg)

    def error(self, msg):
        if self._log_lvl > 0:
            print(msg)

    def _insert(self, object_dict_):
        assert type(object_dict_) == dict, 'Wow! Not dict!'
        self._memory.append(object_dict_)

    def sync(self):
        f = open(self._db_name, 'wb')
        for d in self._memory:
            for name_, type_, len_ in self._schema:
                bytes_bytes = DUMPERS[type_](d[name_], len_)
                f.write(bytes_bytes)
            f.write('\n')
        f.close()

        self._memory = self.init_mem(self._db_name, self._schema)

    def execute(self, row):
        """
        Execute SQL query. Support: select, delete, insert.
        """
        # row = "select where value[1] == 'd' and id >= 0 limit 2"
        # row = "insert into ... values (2, 'awdwa')"
        # row = "delete where ... limit k"
        try:
            method, tail = row.split(' ', 1)
            method = method.lower()
            tail = tail.strip(' ')
            rez = None

            self.info(u'-- SQL {0} {1} --'.format(method, tail))

            if method == 'insert':
                r = re.compile(r'^.*?values?[ ]*(\(.*?\))$', re.M)
                z = r.match(tail)
                if z:
                    rez = [self.insert(*z.groups())]
                    if self.auto_sync:
                        self.sync()

            elif method in ['select', 'delete']:
                r = re.compile(r'^.*?(?:(?:where)[ ]*(.*?))?[ ]*(?:limit[ ]*(\d+))?[ ]*([dD][Ee][ScCs][ckCK])?[ ]*$')
                z = r.match(tail)
                if z:
                    rez = self.__getattribute__('go_go')(method, z.groups())
                else:
                    rez = self.__getattribute__(method)()
            elif method == 'last':
                rez = [self.last()]

            if hasattr(rez, '__len__') and rez.__len__() == 1:
                return rez[0]

            return rez
        except Exception as e:
            self.error("Invalid SQL syntax detected: {0!r} by {1}".format(row, e))
            raise Exception('Invalid SQL syntax!!')

    def go_go(self, method, args):
        return self.__getattribute__(method)(*args)

    def delete(self, where=None, limit=None, desk=None):
        limit = int(limit.strip()) if limit else 0
        where = 'True' if not where else where
        where = self.fix_where(where)

        rez = 0
        del_indexes = []
        l = locals()
        i = 0
        mem = self._memory if not desk else reversed(self._memory)
        for d in mem:
            for name_, type_, len_ in self._schema:
                l[name_] = d[name_]

            st = parser.expr(where)
            is_ok = eval(st.compile())

            if is_ok:
                rez += 1
                del_indexes.append(i)

            i += 1

            if limit and rez >= limit:
                break

        z = 0
        for x in sorted(del_indexes):
            self._delete_dy_index(x - z)
            z += 1

        return rez

    def _delete_dy_index(self, index):
        if 0 <= index < len(self._memory):
            del self._memory[index]
            return 1
        return 0

    def _delete_dy_indexes(self, *indexes):
        del_counter = 0
        for index in sorted(indexes):
            deleted = self._delete_dy_index(index - del_counter)
            del_counter += deleted
        return del_counter

    def _memory_dump(self):
        print '\n-- dump --'
        for d in self._memory:
            print d.values()
        print '-- |--| --\n'

    def select(self, where=None, limit=None, desk=None):
        limit = int(limit.strip()) if limit else 0
        where = 'True' if not where else where
        where = self.fix_where(where)

        rez = []
        l = locals()
        mem = self._memory if not desk else reversed(self._memory)
        for d in mem:
            for name_, type_, len_ in self._schema:
                l[name_] = d[name_]

            st = parser.expr(where)
            is_ok = eval(st.compile())

            if limit and len(rez) >= limit:
                return rez

            if is_ok:
                rez.append(d)
        return rez

    def insert(self, insert_obj_row):
        if not insert_obj_row.startswith('(') or not insert_obj_row.endswith(')'):
            return
        try:
            st = parser.expr(insert_obj_row)
            obj = eval(st.compile())

            if type(obj) != tuple or len(obj) != len(self._schema):
                return

            d = {}
            i = 0
            for name_, type_, len_ in self._schema:
                d[name_] = obj[i]
                i += 1

                if type(d[name_]) != type_:
                    return

            self._insert(d)
            return d
        except Exception as e:
            self.error('Insertion error!', e)
            return

    def fix_where(self, where):
        return where.replace(' = ', ' == ')

    def last(self):
        pass

    def init_mem(self, db_name, schema):
        i = 0
        all = []
        data = {}

        def read_tail(f):
            while True:
                c = f.read(1)
                if not c or c == '\n':
                    return

        try:
            _r = open(db_name, 'rb')
        except:
            self.info("Create New DB")
            try:
                t = open(db_name, 'a')
                t.close()
            except Exception as e:
                self.error("Can not create NEW DB", e)
                raise Exception('Can not create new DB')

        try:
            _r = open(db_name, 'rb')
        except:
            self.error("I/0 Error #1! Can not open!")
            raise Exception("I/0 Error #1! Can not open!")

        while 1:
            if i == len(schema):
                all.append(data)
                read_tail(_r)
                i = 0
                data = {}

            name_, type_, len_ = schema[i]

            d = _r.read(len_).replace('\0', '')
            if not d: break

            zero = _r.read(1)
            if not zero or ord(zero) != 0:
                read_tail(_r)
                i = 0
                data = {}
                continue

            try:
                data[name_] = type_(d)
            except:
                read_tail(_r)
                i = 0
                data = {}
            else:
                i += 1

        _r.close()

        return all


if __name__ == "__main__":
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

    # s._memory[0]['id'] = 22
    # s._memory[0]['value'] = 'adawd'
    s.sync()
