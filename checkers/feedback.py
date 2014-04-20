#!/usr/bin/python3

import uuid
import random
import string
import requests as r

from httpchecker import *

GET = 'GET'
POST = 'POST'
PORT = 7654

class FeedbackChecker(HttpCheckerBase):
	def session(self, addr):
		s = r.Session()
		s.headers['User-Agent'] = 'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:28.0) Gecko/20100101 Firefox/28.0'
		s.headers['Accept'] = '*/*'
		s.headers['Accept-Language'] = 'en-US,en;q=0.5'
		return s

	def url(self, addr, suffix):
		return 'http://{}:{}/{}'.format(addr, PORT, suffix)

	def parseresponse(self, response):
		try:
			if response.status_code != 200:
				raise HttpWebException(response.status_code)
			try:
				result = response.json()
				#self.debug(result)
				return result
			except ValueError:
				raise r.exceptions.HTTPError('failed to parse response')
		finally:
			response.close()

	def spost(self, s, addr, suffix, data = None):
		response = s.post(self.url(addr, suffix), json.dumps(data), timeout=5)
		return self.parseresponse(response)

	def sget(self, s, addr, suffix):
		response = s.get(self.url(addr, suffix), timeout=5)
		return self.parseresponse(response)

	def randword(self):
		word = ''
		rnd = random.randrange(2,10)
		for i in range(rnd):
			word += random.choice(string.ascii_lowercase)
		return word

	def randphrase(self):
		phrase = ''
		rnd = random.randrange(1,4)
		for i in range(rnd):
			phrase += ' ' + self.randword();
		return phrase.lstrip()

	def check(self, addr):
		s = self.session(addr)

		vote = {'title':self.randphrase(), 'text':self.randphrase(), 'type':'visible'}
		self.debug(vote)

		result = self.spost(s, addr, 'put', vote)
		if not result or result.get('error'):
			print('put msg failed')
			return EXITCODE_MUMBLE

		result = self.sget(s, addr, 'search?query=' + vote.get('title') + ' ' + vote.get('text'))
		if not result or result.get('error'):
			print('search failed')
			return EXITCODE_MUMBLE

		if not (result.get('hits') > 0):
			print('posted msg not found')
			return EXITCODE_MUMBLE

		return EXITCODE_OK

	def randlogin(self):
		return random.choice([
			'idiocyxy','x3ajgnkrs13xtz','zavzema98','AssupRummubkt','empinhatn1','xxthinlovexxve','nagevolg05','Prahunmahvv',
			'zgurenimbn','valentnihnj','anonikelavs','Boutibi8q','mitelemkog5','atomize8v','etnologu1o','ze2ta2c8v',
			'nakrenu46','Scheuchea9','aefoxwillowqn','gollwngd1','aldo92200tg','pasjomny','strnoutjw','magsledefm',
			'bybelboekay','canaletto4v','sabinabr1234f','nagicamfr','trzaladh','nosie8','lustrous0v','nullvekst5t',
			'trissadork1','heynes2r','j3s2tgk','vises1b','maemotozq','ata2lapb6h','britneysboobspy','tomaiajl',
			'washaufurw','partieierqz','Math20','Ogulinkahny','pvillalobos2jm','rhagrasyskh','pinaybluele','aprilbloglp',
			'humorneur','obtahovatwp','agrafatgeuk','Entlerfp','BPOws','a2p1op1t26b','uchelafhc','crmdp',
			'mhuinntirqr','cohitarue','teampullzr','operculan7v','Ladidu6','MirmErronee62','errollawdz','searahlj',
			'g1intjy7','Alpenklubmj','Ektropion89','Auswuchsa5','freveltatua','o1kia5','iconhaveitxm','estrenhutfo',
			'Dipnainnaof','surras8b','quotexotasticvz','queculturaji','trueblonde71111','brigidashwoodrx','dreamfocusvy','kloptenzd',
			'ardalhatskd','supersayayin1n','glittery73bo','WeannamuPuv','masikipah','elblogdeinsai5w','Pedaleriek1','gbvideod3',
			'dragonhotnewsjt','FoumeVosem5','demagogisz3','vojnikev8','Sernau0h','luydduuf','orablerpaxopsc8','wrmbulletinoe',
			'lobaretzdh','ladyandria8g','madcow1041hc','malletad1','Elvisiorb','tenorytd8','angelicsnd','aber1r4',
			'2t1ar1cor2','Runtimew1','Ticenvibexh','zagabeeniee7','wokufaxj','gestormdhw','rezilcenn','duizelingr4',
			'majuzibx','deedee1307b1','pinkcheerios5g','gsusfreekdawnu6','konfetx1','nemizacaz8','klimopbos3m','ko2r1asztwp',
			'joelleferrete6y','iijokuoi0f','apozeugmasn','previsyh','pensionarja','Gaiarsanm','Neuansatzn3','burroughsman3l',
			'milenimvl','unapred3l','dekuncicy','Dawudte','smutnyrv','monsmons10','Domiblooria6z','dodawanies8',
			'torrent11p','hatedwords9x','aluhimrjn60','BopilkToola9j','ventverts9','Simeoli90','beiblatt8g','deposanrz',
			'Farbeimeroq','nijemca9x','ratelafq','wangelia777k0','Galestabs3c','o3mundosq5','lepidofitlx','mattdrew18qk',
			'agotaretzky','nakedsilencehd','jothepigy2','paiso1u','sjekteap','globojapi','pascut7v','desparetezx',
			'elegikerl7','agresszorci','dwarrowchild07','pomerilif2','maksillei9','Scibelliuh','Puncovhh','arturoonorio8q',
			'gmit6m','Briessekildrx','stebrcebr','WedLayexiaLabxc','tipicizezg1','Warburgerql','Moospadvavasf','NekOthepeke',
			'throwww','crogailln3','groorseZoog1v','Hurenbock1h','fuktigty','raspuste6z','bonkalw','varaularvn',
			'lemoncleansepm','Cabrol6b','frutseldemt','jujumevi2y','enzo90910sm','halfboy3y5','envadidorbr','flapkanre',
			'slavenkomw0','Rucmancemhe','CYMNBYNCVOK2h','z2jmw','entricasn8','clumsyshakess','wimberg04','ystormqz',
			'rollrgurl31zh','naklapalogp','Penzify','sxxxybaby1231y','marmoteisbv','uloviju1k','encoresseh6','crecianin',
			'jetledeeshorsdr','glockech','aidee18spainy1','BouswrareeCorsm','wingtsang2009s6','miseriaphyn97','xliddoxangelx2m','Erartooheriaf8d',
			'kilichokol2','garoxb','depletiv46','wudabumha','o2e1maij','Roulett02','ashelleystar3g','Trnovemuu4',
			'Csato00','betisinfodw','timexturner6v','lurkumjk','prvoborecvl','Pektenosetw','agulhoat0c','bancaladan8',
			'ocotlan2e','st4ie6j','Cohesdps','Jozzinofe','Eltzetj','edumnRogdregoms','tandwerk7b','keberioti6r',
			'konzilijex6','xskallelujahxa1','Szramowomu','pinksoda30084','garguiletz','disco2discoze','asignatk1q','DemOrenueDookqr',
			'yeggmh','bailaratzmo','zasutimaf8','Volksmund33','Dirmintiqg','wynwynyh','biollaiodiblegy','secretaresp',
			'tresutsla','insuranxnns5','werekanace','Varvellixy','nashinandaz3','preluderoninnl','aturdido1c','przybitka56',
			'vedricom4w','visualedgare1','Anagenese5z','Waldteilv1','fylliligavs','parsiemtijq','Affeflytemy57','maddi6665r',
			'mshihirix5','renareneli','Gnevsdorf0v','Cerigato8w','jrpantalim','mejicanofv','lanosasjn','thehulkstoygw',
			'acard39','tulpenbed9c','bamazeiz','Gezirph4','szmerekgr','atkilusqv','USAswk','aveusidaspw',
			'Karftwr','rhen21083u','Edelherrb5','sklonitexi','bbostate200u','premostila3','Hentzels9','chentexblogg8',
			'nebhandeib','stormdeib','illichbuilsn8','uilogymk','dyftykxu','custodianrg','rotace2q','spaposteodsvi',
			'iskrcaloo7','Iantaffi0f','goriStoottewu','pe3r2onas','misterjsonqi','newnightmarete','friend17ix','lojanoih',
			'Floressele','emali49','feblaire85','Arameespr','Pleldcerdemx0','turjainj','clairelascauxbd','bankobrevun',
			'nibris9a','quoteiliciouss6','angfersure2q','gwmpasall','Opponacamicoxav','magolents4l','abgeguckt5v','nemirnim82',
			'Attinsvenc0','Bikinic4','hollvini9w','sz3s2rx3','Miragalliw5','droitdusagev3','toady12bo','amrantunas9',
			'Skactweewvv','Steandodofadeea','exharacamni','umerilort5','guindarql','letopisom04','sars0116jd','odsiewaczgp',
			'Laxaxiovab2','CorsCoide0k','phasiwedeuf','sebansow9','papitosolteroj7','webpensieroul','puposoj7','Geraultan',
			'DymnkeypeDoffbi','Algownegorigejn','umniniyofx','romporaxe','misvan93mn','inchex','conger39','Husckovickf',
			'minciunowp','thepookguyt0','becknay5','Thurner3u','zasipanojav','fresalyz','lagosphere1g','kesiaaaaa9l',
			'emuttogeddyft','ae2pezi','gebuttertuz','glukozamixr','Pugljemo6','pembiusmv','Partitam4','unaniemut',
			'Seekanneds','malungelowh','AR8oy','van3000q5','Caile9w','imacephelmKamfn','cotsant3v','hakutuliayg',
			'imbornalk1','msgpusherc5','ghionoie2','Scabbiolo2r','Azad4o','espadelatke','itiparanoid13h3','K8CTZAE0k',
			'j0j0fansyb','elcalvoinvitabb','izmailicimv','ersatzub','periheliuh3','rovovskadi','seguicollaraw','rhodib2',
			'Kandoraeb','ariup3t','Musarjev9b','Kindhardri','diarejamihw','sunandgamesii','krypinaje','drip21lc',
			'fajansamal9','Saipan3s','scenarijacm','tubrhrkz9','fraktinj8','oporowieczm','Galfanozr','miquelcasals0k',
			'kuranskoj2i','rovito6d','venteprimo1','e3rumqa','istrebihbj','trebuiau3g','atupatzbq','Irraragocakepdt',
			'sintagmamii','bi1laeh','5pawkf','vaginmw','pastelcupcakeok','zeefactorf8','severuluieg','crysenc5',
			'matresrz','Tyncdercard9z','turdidesaz','gewelnaam23','hinziehen2i','pegreedilydaynw','pazaroll5','comattaip6g',
			'Wicewoo7','ipicaslj','rehabyuo','heiliasetfx','mediasferasie','Doonvatantanow1','luxatsu2','discounsyxv8',
			'longiadusl','darkdirkre','zagorze6g','ffiwdal8h','remirava85','Santiannif6','virekipirb','dukinoge2',
			'aeast317xe','blogmgqq','greenteaxoa9','Kompasuum','skourosekj','dekadischsh','go2r1e2lse','predisent4y',
			'exile23pt','rishigoqp','loiswanglingsz','zegget3','espirant9g','Paulinawf','mesmotsblogla','dukeblue3100tk',
			'veldua0','Engakycanny15','protkanu7r','islamomv3','WibleDumDoobej5','marcadius27','sajastegai6','codothi',
			'ParlFedaTarieca','Remorinoyg','Cagnolato63','doceerde25','seneVemeshorm04','encrestanmo','letst5k','pus1an2yjw',
			'desbardisi8','Eleltyzexxm','rudejaset','taffy1957u7','xoxobabes215h3','1JF6W5TO35','namahac6','dakgrasps',
			'deeltji','Zanollo8o','ubervolksmd','alyreneeqn','SnulleynC92','Kiestra7b','wilfredmong24','Jizba2e',
			'decadere9w','rispiglp','sammiebugev','Fexwoossyb1','uthiwea2','rutelk0','migradazf','Garitox0',
			'teletipesgg','mitomansb7','Belloneuf','thoirh3','krizoji5','elcabogne4w','pengeverdzo','estivatzo5',
			'propirioxt','chamuyamosargen','silverhillmv','osmotriteqg','il1blqr','kieuholtevt','odgojnojyj','mybsterw3',
			'Grabeland5w','laminato8s','brukkencp','izrecitewj','journaldexcogi','dekadamidz','recargatzpt','musivischf9',
			'glavarom8h','aubavaqf','lildrummagurluv','dorkogrande16pw','ruthieberk7o','reapudembr','Bacciani88','azizaju',
			'yndislegu84','niedurno4g','freakypryncez79','binnetrekvx','Realduccels02','Bleifarbev7','Affeldleaps00','caitgoesrarr6i',
			'skater7170vw','pokrovcemcv','Llangoeddy','Armeenjerxd','falquear77','rasierqv','Saraspitsn3','anmelde1r',
			'Faltenbaunu','aparentas2b','sur3n417z','filtrable2m','myescaperoute6s','moldrokixn','Prascafh','Franckinbw',
			'cyllellh8','klargjerecy','iwapoj2','cymhellom3b','akaofg','rabanhejp','Osmundw9','nazizm3w','Capuni8c',
			'poseermei6','noilliormem70','oplugtingye','szaladhat43','drpsalk6','nomerompandw','mornege2q','Aasmundc0',
			'Geraunzgy','Minotaurnw','firehead41vn','zuwettern3r','Lassig0e','lainemoutaineaq','club85uk','sachouuz',
			'timelipp9x','oscinmemn3e','zaprekama4k','gudangm4','dyemeng8','shaltairt5z','zingarsll'
		])

	def randtitle(self):
		return random.choice([
			'Interesting note for you',
			'Good news!',
			'What, what, what?!',
			'Thanks',
			'Good service',
			'Question...',
			'Cooooooooool',
			'We have a question',
			'Does it really work?',
			'What`s the longest you have gone without sleep?',
			'How to get it?',
			'Check this, please',
			'Did you see it?!',
			'WOW!',
			'Amazing!!!11',
			'Coolest thing ever',
			'Some stuff here',
			'Hmmm....',
			'May be you can check it for us?',
			'Can you answer our question?',
			'Got it!',
			'Booooooooom!!!',
			'Pwned!',
			'Something interesting...',
			'To be or not to be?',
			'... not to be',
			'You should look at this',
			'Look at this',
			'Meeeeeoooooooow',
			'Did you find any flag here?',
			'Das ist gut',
			'Hello and Welcome aboard'
		])

	def randuser(self):
		login = uuid.uuid4().hex
		passlen = random.randrange(8, 14)
		password = uuid.uuid4().hex[:passlen]
		return {'login':self.randlogin() + ' ' + login, 'password':password}

	def randvote(self, flag_id, flag):
		rnd = random.randrange(2)
		title = self.randtitle()
		return {'title':title, 'text':flag, 'type':'invisible'}

	def get(self, addr, flag_id, flag):
		s = self.session(addr)

		parts = flag_id.split(':', 3)

		user = {'login':parts[0], 'password':parts[1]}
		result = self.spost(s, addr, 'auth', user)
		if not result or result.get('error'):
			print('login failed')
			return EXITCODE_MUMBLE

		self.debug(parts[2])
		result = self.sget(s, addr, 'search?query=' + parts[2])
		if not result or result.get('error'):
			print('search failed')
			return EXITCODE_MUMBLE

		if result.get('hits') < 1:
			print('posted msg not found')
			return EXITCODE_CORRUPT

		votes = result.get('votes')
		if not votes or len(votes) == 0:
			print('posted msg not found')
			return EXITCODE_CORRUPT

		title = votes[0].get('title')
		text = votes[0].get('text')
		if not ((title and title.find(flag) >= 0) or (text and text.find(flag) >= 0)):
			print('flag not found in msg')
			return EXITCODE_CORRUPT

		return EXITCODE_OK

	def put(self, addr, flag_id, flag):
		s = self.session(addr)

		user = self.randuser()
		self.debug(user)

		result = self.spost(s, addr, 'register', user)
		if not result or result.get('error'):
			print('registration failed')
			return EXITCODE_MUMBLE

		vote = self.randvote(flag_id, flag)
		self.debug(vote)

		result = self.spost(s, addr, 'put', vote)
		if not result or result.get('error'):
			print('put msg failed')
			return EXITCODE_MUMBLE

		data = result.get('data')
		if not data:
			print('no put result there')
			return EXITCODE_MUMBLE

		new_flag_id = data.strip()
		if not new_flag_id:
			print('no put result there')
			return EXITCODE_MUMBLE

		print('{}:{}:{}'.format(user['login'], user['password'], new_flag_id))
		return EXITCODE_OK

FeedbackChecker().run()