How Use
-------

    virtualenv venv

    .\venv\Script\activate # for windows!
    .\venv\bin\activate # for other!

    pip install -r requirements.txt

    python f_mash.py

Hack it
-------

    127.0.0.1 - - [03/Apr/2014 18:54:46] "GET / HTTP/1.1" 200 -
    insert into messages values ('qwe\', 2, ',) + (str(2),"hhh") #')
    -- SQL insert into messages values ('qwe\', 2, ',) + (str(2),"hhh") #') --
    insert!

Hacks
-----
 - DEBUG = True - python code injection!
 - /ack/ - open for all last flags.
 - /post/ - blind python injection!
 - /post/ - db store format error, can add 2 rows.

interesting
-----------


   [c for c in ().__class__.__base__.__subclasses__() if c.__name__ == 'catch_warnings'][0]()._module.__builtins__
