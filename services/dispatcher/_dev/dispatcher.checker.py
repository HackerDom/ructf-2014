#!/usr/bin/env python2
# coding=utf-8
import pickle
import random
import sys
import re
import base64

__author__ = 'pahaz'

CALLSIGNS = [
    ' Aurora ', ' Azar ', ' Azorin ', ' Aktivar ', ' Scarlet ', ' Alfur ', ' Amazar ', ' Amba ', ' Ambarchik ',
    ' Empire ', ' Antiques ', ' Arbat ', ' Argan ', ' Harim ', ' Assistant ', ' Atlanta ', ' Atlantis ', ' Atropine ',
    ' Bagrets ', ' Bad ', ' Badion ', ' ATM ', ' Bank ', ' Bakhar ', ' Bashkir ' ' Bezbrezhnos ', ' Berezhnoj ',
    ' Berezhnev ', ' Bisector ', ' Bunker ', ' Hangover ', ' Mash ', ' Brother ', ' Cranberry ', ' Clock ', ' Hedwig ',
    ' Bulyha ', ' butylene ', ' Vaseline ', ' Wajda ', ' Shaft ', ' Roll ', ' Veerochek ', ' Vertical ', ' Branch ',
    ' Revolution ', ' Beijing ', ' Fell ', ' Geyser ' ' Hertz ', ' Hydrograph ', ' Hydrological ', ' Hydrolysis ',
    ' Chestnut ', ' Little Voice ', ' Maiden ', ' Praises ', ' Crusher ', ' VFB ', ' Doudney ', ' Dunel ', ' Dunis ',
    ' Spruce ', ' Yerofeyich ', ' Ershov ', ' Plaintive ', ' Ji ', ' Drop ', ' Cherished ', ' Plowing ', ' Act ',
    ' Perky ', ' Zimush ', ' Zootechnician ' ' Finch ', ' Igrets ', ' Ideological ', ' Elected ', ' Exile ',
    ' Surplus ', ' Propertied ', ' Adversary ', ' Edges ', ' Kazachek ', ' Cactus ', ' Kamenka ', ' Capel ', ' Elm ',
    ' Painting ', ' Katyusha ', ' Kipazh ', ' Clan ', ' Klumbochka ', ' Kovriga ', ' Comet ', ' Korsak ', ' Drupe ',
    ' Cuckoo ', ' Kulan ' ' Lapat ', ' Eraser ', ' Latvian ', ' Swan ', ' Linen ', ' Ladder ', ' logarithm ', ' Shred ',
    ' Hole ', ' Lute ', ' Marianka ', ' Marina ', ' Marinade ', ' Marinovka ', ' Flywheel ', ' Makhova ', ' Melody ',
    ' Merochka ', ' Mechanic ', ' Whisk ', ' Brooms ', ' Monolith ', ' Monarchist ', ' Mimosa ', ' Nadel ' ' nakida ',
    ' Servage ', ' Gherkin ', ' Odulyar ', ' Oxide ', ' Casting ', ' Oratorio ', ' Orkut ', ' Panza ', ' Ferry ',
    ' Scrolling ', ' Transporter ', ' Overgrown ', ' Peresloyka ', ' Sill ', ' Horseshoe ', ' Duel ', ' Floodplain ',
    ' Sing ', ' Commander ', ' Half ', ' Poltavka ', ' User ', ' Trustee ', ' Port ' ' Privada ', ' Prisyadka ',
    ' Prozhatok ', ' Strait ', ' Break ', ' Prolomnaya ', ' Spacers ', ' Radan ', ' Hall ', ' Resident ', ' Thread ',
    ' Redan ', ' Santim ', ' St ', ' Sardines ', ' Shackle ', ' Gray ', ' Sinigrin ', ' Synoptics ', ' Sip ',
    ' mackerel ', ' Regular ', ' Stenographer ', ' Stereoscope ', ' Shy ' ' Fellowship ', ' Small stands ',
    ' Strategy ', ' Stroevik ', ' Taft ', ' Taima ', ' Thistle ', ' Solid ', ' Creator ', ' Locomotive ', ' Grated ',
    ' Tesla ', ' Tina ', ' Titina ', ' Tom ', ' Surveyor ', ' Torba ', ' Face ', ' Tosno ', ' Sharpener ', ' Track ',
    ' Tripod ', ' Triplet ', ' Trolley ' ' Tropical ', ' TNT ', ' Tyurik ', ' Sanctuary ', ' Overture ', ' Way ',
    ' Ukrain ', ' Utruzhka ', ' Courteous ', ' Tub ', ' Banufacturer ', ' Fanerku ', ' Fargo ', ' Fatalist ',
    ' Phenology ', ' Forez ', ' Fortune ', ' Photometer ', ' Chrysanthemum ', ' Tzadik ', ' CAP ', ' Center ',
    ' Celandine ', ' Jumper ', ' Spur ' ' Head ', ' State ', ' Shtafirka ', ' Plug ', ' Bayonet ', ' Train ',
    ' Electric ', ' Emba ', ' Energetic ', ' Erki ', ' Expander ', ' Eskimo ', ' Young ', ' Bright '
]
MESSAGE_FORMATS = [
    'I tell you that {0}',
    '{0} is important for you',
    'Please {0}',
    'We now that {0}',
    'Hi, {0}',
    'Interesting, {0}',
]

'''
check - проверка общей функциональности
put - установка флага в сервис
get - получение флага из сервиса

Чекер может использовать поток STDERR для вывода собственных диагностических сообщений.

STDOUT может использоваться для:

вывода пояснений командам о причине неработоспособности сервиса (если код завершения не равен 101)
вывода нового идентификатора флага (в режиме "put", если код завершения равен 101)
Причина неработоспособности (напр., "Connection refused", "Unexpected answer from service", ...) будет отображаться всем командам на странице скорборда с текущими статусами сервисов.

'''

import requests as r


class Context(object):
    pass


class Packer(object):
    @staticmethod
    def pack(context):
        return base64.b64encode(pickle.dumps(context))

    @staticmethod
    def unpack(string):
        string = base64.b64decode(string)
        return pickle.loads(string)


class AbstractTransport(object):
    transport_exceptions = []

    def is_transport_exception(self, e):
        is_e_instance_of = lambda x: isinstance(e, x)
        return any(map(is_e_instance_of, self.transport_exceptions))

    def __init__(self, host):
        self.host = host



class Checker(object):
    port = 5000
    packer = Packer

    def __init__(self, host):
        self.host = host
        self.http_init()

    def put(self, id, flag, *args):
        '''
        PUT: установка флага.
        ./checker put <hostname> <id> <flag>

        Чекер должен установить флаг <flag> на хост <hostname>.

        В случае успешной установки (код возврата 101 - ОК) чекер может вывести на STDOUT новый идентификатор флага. В этом случае проверяющая система занесет в базу флагов новый идентификатор, а не . Новый идентификатор может состоять из нескольких строк. Если на STDOUT ничего не выведено, считается, что флаг установлен под идентификатором <id>, изначально предложенным проверяющей системой.

        Проверять, установился ли флаг (т.е. пытаться его получить из сервиса), чекер в этой процедуре не должен. Это сделает проверяющая система сразу после вызова "put".
        '''
        try:
            context = Context()
            context.flag = flag

            self.do_before_post_flag(context)
            self.do_post_flag(context)
            self.save_state_in(context)

            flag_id = Checker.packer.pack(context)
            if len(flag_id) > 1000:
                self.status_mumble('many cookies!')

            self.status_ok(flag_id)
        except r.RequestException:
            self.status_down()
        # except r.Timeout:
        #     self.status_mumble('Timeout')

    def check(self, *args):
        '''
        CHECK: проверка общей функциональности.
        ./checker check <hostname> Чекер должен проверить общую функциональность сервиса.
        Если чекеру не нужна такая проверка, он возвращает 101 ("OK").
        '''
        try:
            context = Context()

            self.do_before_post_flag(context)

            self.status_ok()
        except r.RequestException:
            self.status_down()
        # except r.Timeout:
        #     self.status_mumble('Timeout')

    def get(self, id, flag, *args):
        '''
        GET: получение флага.
        ./checker get <hostname> <id> <flag>
        Строка <id> может содержать пробелы и переводы строк. Вся строка целиком будет лежать в argv[3].
        Чекер должен проверить: можно ли по идентификатору <id> получить флаг <flag> с хоста <hostname>.
        '''
        try:
            context = Checker.packer.unpack(id)
            if context.flag != flag:
                self.status_error('(unpacked flag from `id`) != (`flag` from argv)')

            self.load_state_from(context)
            self.do_after_post_flag(context)

            self.status_ok()
        except r.RequestException:
            self.status_down()
        # except r.Timeout:
        #     self.status_mumble('Timeout')

    def url_for(self, name='index', arg=None):
        if name == 'is_free_frequency':
            method = 'is_free_frequency/' + str(arg) + '/'
        elif name == 'index':
            method = ''
        elif name == 'post':
            method = 'post/'
        elif name == 'ack':
            method = 'ack/'
        elif name == 'dialog':
            method = 'dialog/' + str(arg) + '/'
        else:
            raise Exception('Invalid url.')

        host = self.host
        port = self.port
        return 'http://{host}:{port}/{method}'.format(**locals())

    def status_down(self):
        """104 - DOWN сервис не работает (напр., нет connect-а на порт)"""
        sys.exit(104)

    def status_mumble(self, public_message=''):
        """103 - MUMBLE сервис работает неправильно (напр., отвечает не по протоколу)"""
        print(public_message)
        sys.exit(103)

    def status_error(self, error_msg):
        """110 - CHECKER ERROR внутренняя ошибка чекера (напр., неправильные аргументы командной строки)"""
        self.log_error(error_msg)
        sys.exit(110)

    def status_corrupt(self):
        """102 - CORRUPT сервис работает, но запрашиваемого флага нет (только в режиме «get»)"""
        sys.exit(102)

    def status_ok(self, flag_new_id=None):
        """101 - ОК (операция завершилась успешно)"""
        if flag_new_id:
            print(flag_new_id)

        sys.exit(101)

    def do_before_post_flag(self, context):
        rez = self.http_get(self.url_for('index'))
        if rez.status_code != 200:
            self.status_mumble("/ url - invalid status")

        frequency = self.do_getting_valid_frequency()

        context.frequency = frequency

    def do_getting_valid_frequency(self):
        valid_frequency = None
        for x in range(5):
            frequency = self.generate_frequency()
            rez = self.http_get(self.url_for('is_free_frequency', frequency))
            if rez.status_code != 200:
                self.status_mumble("/is_free_frequency/ url - invalid status")

            if rez.text == 'True':
                valid_frequency = frequency
                break
            elif rez.text == 'False':
                pass
            else:
                self.status_mumble("/is_free_frequency/ url - return invalid value! Required only True/False")

        if not valid_frequency:
            self.status_mumble("/is_free_frequency/ url - can't find valid frequency")

        return valid_frequency

    def http_init(self):
        self._cookies = {}

    def http_get(self, url):
        cookies = self._cookies
        headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.116 Safari/537.36'}
        rez = r.get(url, timeout=1, cookies=cookies, headers=headers)
        self._cookies = rez.cookies.get_dict()
        return rez

    def http_post(self, url, data):
        cookies = self._cookies
        headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.116 Safari/537.36'}
        rez = r.post(url, data, timeout=1, cookies=cookies, headers=headers)
        self._cookies = rez.cookies.get_dict()
        return rez

    def do_post_flag(self, context):
        context.message = self.generate_message_with_flag(context.flag)
        context.callsign = self.generate_callsign()
        rez = self.http_post(self.url_for('post'), data={
            'frequency': context.frequency,
            'callsign': context.callsign,
            'message': context.message,
        })
        if rez.status_code != 200:
            self.status_mumble('/post/ url - invalid status')

        if '<form' not in rez.text or '</form>' not in rez.text:
            self.status_mumble('/post/ url - answer did not contain form')

        if '<h1>True</h1>' not in rez.text:
            self.status_mumble('/post/ url - answer did not contain True')

        # ack !
        rez = self.http_get(self.url_for('ack'))
        if rez.status_code != 200:
            self.status_mumble('/ack/ url - invalid status')

        if context.message not in rez.text:
            self.status_mumble('/ack/ url - answer did not contain message')

        rez = self.http_get(self.url_for('index'))
        if rez.status_code != 200:
            self.status_mumble('/ url - invalid status')

        if str(context.frequency) not in rez.text:
            self.status_mumble('/ url - did not contain my post frequency')

    def http_get_cookies(self):
        return self._cookies

    def http_set_cookies(self, cookies):
        self._cookies = cookies

    def do_after_post_flag(self, context):
        rez = self.http_get(self.url_for('dialog', context.frequency))
        if rez.status_code != 200:
            self.status_mumble('/dialog/ url - Invalid status')

        if context.message not in rez.text:
            self.status_corrupt()

    def log(self, msg, prefix='', postfix=''):
        sys.stderr.write('{0}{1}{2}\n'.format(prefix, msg, postfix))

    def log_error(self, error_msg):
        self.log(error_msg, " ** [ERROR] IMPORTANT ** ")

    def save_state_in(self, context):
        context._cookies = self.http_get_cookies()

    def load_state_from(self, context):
        self.http_set_cookies(context._cookies)

    def generate_frequency(self):
        return str(random.randint(10000000000, 99999999999))

    def generate_callsign(self):
        return random.choice(CALLSIGNS)

    def generate_message_with_flag(self, flag):
        format_str = random.choice(MESSAGE_FORMATS)
        return format_str.format(flag)


if __name__ == '__main__':
    command = sys.argv[1].lower()
    if command not in ['put', 'get', 'check']:
        sys.exit(110)

    host = sys.argv[2]
    args = sys.argv[3:]

    c = Checker(host)
    command = getattr(c, command)
    command(*args)

    # c = Checker('127.0.0.1')
    # # c.check()
    # # c.put('alfnawgnawlgnawlfnaw', 'a2a2a2e2e2e2r22r2r2r')
    # c.get(
    #     "ccopy_reg\n_reconstructor\np0\n(c__main__\nContext\np1\nc__builtin__\nobject\np2\nNtp3\nRp4\n(dp5\nS'_cookies'\np6\ng0\n(crequests.cookies\nRequestsCookieJar\np7\ng2\nNtp8\nRp9\n(dp10\nS'_now'\np11\nI1397666210\nsS'_policy'\np12\n(icookielib\nDefaultCookiePolicy\np13\n(dp14\nS'strict_rfc2965_unverifiable'\np15\nI01\nsS'strict_ns_domain'\np16\nI0\nsS'_allowed_domains'\np17\nNsS'rfc2109_as_netscape'\np18\nNsS'rfc2965'\np19\nI00\nsS'strict_domain'\np20\nI00\nsg11\nI1397666210\nsS'strict_ns_set_path'\np21\nI00\nsS'strict_ns_unverifiable'\np22\nI00\nsS'strict_ns_set_initial_dollar'\np23\nI00\nsS'hide_cookie2'\np24\nI00\nsS'_blocked_domains'\np25\n(tsS'netscape'\np26\nI01\nsbsg6\n(dp27\nS'127.0.0.1'\np28\n(dp29\nS'/'\np30\n(dp31\nS'session'\np32\n(icookielib\nCookie\np33\n(dp34\nS'comment'\np35\nNsS'domain'\np36\nS'127.0.0.1'\np37\nsS'name'\np38\nS'session'\np39\nsS'domain_initial_dot'\np40\nI00\nsS'expires'\np41\nNsS'value'\np42\nS'eyJmcmVxdWVuY3kiOnsiIGIiOiJNelEzTURVM016Y3pNemc9In19.BjBDIg.hDaVDPNit3EXCjdmol-ydbk3Whs'\np43\nsS'domain_specified'\np44\nI00\nsS'_rest'\np45\n(dp46\nS'HttpOnly'\np47\nNssS'version'\np48\nI0\nsS'port_specified'\np49\nI00\nsS'rfc2109'\np50\nI00\nsS'discard'\np51\nI01\nsS'path_specified'\np52\nI01\nsS'path'\np53\ng30\nsS'port'\np54\nNsS'comment_url'\np55\nNsS'secure'\np56\nI00\nsbssssbsS'flag'\np57\nS'a2a2a2e2e2e2r22r2r2r'\np58\nsS'frequency'\np59\nS'34705737338'\np60\nsS'callsign'\np61\nS' Electric '\np62\nsS'message'\np63\nS'Hi, a2a2a2e2e2e2r22r2r2r'\np64\nsb.",
    #     'a2a2a2e2e2e2r22r2r2r')
