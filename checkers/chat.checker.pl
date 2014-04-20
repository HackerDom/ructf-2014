#!/usr/bin/perl
use strict;
use IO::Socket::INET;
use IO::Select;
++$|;

######## Config ########

my $TIMEOUT      = 5;
my $DEBUG        = 1;
my $PORT         = 5555;
my $READ_TIMEOUT = 5;
my $READ_BUF     = 65535;
my $DUMP         = 1;

########################

sub MSG          { my $s = pop; $s =~ s/[\r\n]+/[\\n]/g; $s; }

sub EXIT_OK      { my $s = pop; exit 101 }
sub EXIT_CORRUPT { my $s = pop; warn MSG($s).$/; print $s.$/; exit 102 }
sub EXIT_MUMBLE  { my $s = pop; warn MSG($s).$/; print $s.$/; exit 103 }
sub EXIT_DOWN    { my $s = pop; warn MSG($s).$/; print $s.$/; exit 104 }
sub EXIT_ERR     { my $s = pop; warn MSG($s).$/; exit 110 }

my (@First, @Last);

my %ACTIONS = (
    check => \&check,
    put   => \&put,
    get   => \&get,
);

my $a = shift;
exists $ACTIONS{$a} or usage();
$ACTIONS{$a}->(@ARGV);

exit EXIT_ERR "This should never happen";

####################################################################################################

sub debug {
    warn shift.$/;
}

sub usage {
    EXIT_ERR "Usage: chat.checker.pl check <host> | put <host> <id> <flag> | get <host> <id> <flag>";
}

sub read_data {
    my $mode = '';
    while (<DATA>) {
        chomp;
        if (/^=/) {
            $mode = $_;
        }
        else {
            push @First,$_ if $mode eq '=First';
            push @Last,$_  if $mode eq '=Last';
        }
    }
}

sub connectTo {
    return IO::Socket::INET->new(
        PeerAddr => shift,
        PeerPort => $PORT,
        Proto    => 'tcp',
        Timeout  => $TIMEOUT
    );
}

sub readAll {
    my $sock = shift;
    my $select = IO::Select->new($sock);
    my $result = '';
    while (1) {
        my @ready = $select->can_read($READ_TIMEOUT);
        $sock->recv(my $data, $READ_BUF);
        $result .= $data;
        last if $data =~ '>';
    }
    if ($DUMP) {
        printf STDERR "[ recv ] %s\n", $_ for grep { /\S/ } split /[\r\n]/, $result;
    }
    return $result;
}

sub sendData {
    my ($sock,$data) = @_;
    printf STDERR "[ send ] %s\n", $data if $DUMP;
    $sock->send("$data\r\n");
}

sub assertBanner {
    return shift =~ /::: Welcome to RuCTF 2014 passenger chat :::/;
}

sub check {
    my ($host) = @_;
    my $s = connectTo($host) or EXIT_DOWN $@;
    my $data = readAll($s);
    sendData($s, "\\quit");
    $s->close();
    assertBanner($data) ? EXIT_OK : EXIT_MUMBLE "Welcome banner corrupted";
}

sub put {
    my ($host, $id, $flag) = @_;
    $id =~ s/-//g;

    read_data();

    my $login = sprintf "%s.%s", $First[int rand @First], $Last[int rand @Last];

    my $room  = substr $id, 0, 6;
    my $pass  = substr $id, 6;

    debug "put: generated: $login, $room, $pass";

    my $s = connectTo($host) or EXIT_DOWN $@;

    my $data = readAll($s);
    assertBanner($data) or EXIT_MUMBLE "Welcome banner corrupted";

    sendData($s, "\\register $login $pass");
    $data = readAll($s);
    $data =~ /Login OK/ or EXIT_MUMBLE "register failed";

    debug "register OK";

    sendData($s, "\\create $room $pass");
    $data = readAll($s);
    $data =~ /Welcome to private room '$room'/ or EXIT_MUMBLE "create room failed";

    debug "create room OK";

    sendData($s, "Hello, world!");
    $data = readAll($s);

    debug "say hello OK";

    sendData($s, "$flag");
    $data = readAll($s);

    sendData($s, "\\quit");

    print join "_", $login, $room, $pass;  # New Flag ID
    EXIT_OK;
}

sub get {
    my ($host, $id, $flag) = @_;

    my ($login, $room, $pass) = split '_', $id;

    my $s = connectTo($host) or EXIT_DOWN $@;

    my $data = readAll($s);
    assertBanner($data) or EXIT_MUMBLE "Welcome banner corrupted";

    sendData($s, "\\login $login $pass");
    $data = readAll($s);
    $data =~ /Login OK/ or EXIT_MUMBLE "login failed";

    debug "login OK";

    sendData($s, "\\join $room $pass");
    $data = readAll($s);
    $data =~ /Welcome to private room '$room'/ or EXIT_MUMBLE "join room failed";

    debug "join room OK";

    sendData($s, "\\quit");

    $data =~ /$flag/ or EXIT_MUMBLE "flag not found";

    EXIT_OK;
}

__DATA__
=First
Aaron
Abraham
Adam
Aidan
Alan
Alexander
Alp
Anchor
Andrew
Antony
Ashton
Bailey
Benjamin
Blake
Braden
Bradley
Brady
Brendan
Brent
Brian
Brown
Cade
Cale
Caleb
Calum
Cameron
Carrington
Ceasar
Cheyenne
Christian
Christopher
Cody
Colin
Conner
Connor
Courtney
Dakotah
Damian
Daniel
David
Devin
Domenic
Drew
Dustin
Dylan
Emeri
Emlyn
Erick
Erik
Ethan
Fabien
Frazer
Gabriel
Garfield
Gavin
Gerard
Gideon
Gilbert
Glen
Guy
Harvey
Hayes
Hays
Holden
Ian
Indiana
Isaiah
Isaac
Jackson
Jacob
James
Jared
Jeremy
Jason
Jay
Jesse
Jody
John
Jonah
Jonathan
Joseph
Joshua
Jude
Julian
Justin
Kade
Kaleb
Keala
Keegan
Keith
Kelly
Kelsey
Kendall
Keoki
Kerry
Khalid
Kyle
Landon
Lane
Leif
Leslie
Levi
Liam
Liam
Mackenzie
Madison
Maersk
Malachi
Maliq
Matthew
Michael
Morgan
Nathan
Nathaniel
Neil
Nicholas
Noah
Oakley
O'Bannon
Owen
Parker
Patrick
Paul
Phillip
Quinlan
Rami
Randall
Reece
Richard
Robert
Robin
Ronald
Ryan
Samuel
Scott
Sean
Skyler
Sloane
Stephen
Sutton
Thomas
Timothy
Todd
Trevor
Trey
Trent
Tristin
Tyler
Vance
Victor
Vincent
Virgil
Wayne
Wesley
Westley
Whitley
Wiatt
William
Wilson
Wyatt
Xavier
Zachariah
Zachary
Acea
Ainslee
Alexa
Alixandria
Aliyah
Aleen
Alka
Alyson
Alyssa
Amanda
Amariah
Amarissa
Andrea
Angel
Angela
Anissa
Anna
Annaliese
Ariana
Arielle
Ariene
Arminda
Ashanti
Ashley
Audrey
Audrina
Aurora
Autumn
Bailey
Beatrice
Becky
Belle
Beverley
Bianca
Briana
Brietta
Bronwen
Brooke
Caitlin
Cameron
Cara
Cari
Carin
Carissa
Carolyn
Carrie
Carrigan
Cary
Casey
Cassandra
Cassie
Catherine
Chanda
Chandni
Chanelle
Cheryl
Cheyenne
Chloe
Chrislynn
Cierra
Claudia
Colleen
Corine
Courtney
Daisy
Dakotah
Danica
Danielle
Dara
Daria
Dawnell
Desiree
Destiny
Devin
Dreama
Drew
Dulcie
Eglantine
Elaine
Eliesha
Elizabeth
Elouise
Emeri
Emlyn
Emily
Emma
Erika
Estelle
Evanle
Faith
Gabriella
Giovianna
Grace
Gracie
Hannah
Hannelore
Heather
Helena
Hermanda
Hong
Hope
Iris
Jade
Jamie
Jaime
Janel
Janessa
Jasmine
Jazmin
Jennifer
Jesse
Jessica
Jessie
Jody
Jonah
JosKaitlyn
Katiah
Katie
Kaylie
Kara
Kassidy
Katharyne
Kathleen
Keala
Keira
Kelly
Kelsey
Kendall
Kendra
Kerry
Kiona
Kira
Kirstie
Lacee
Laura
Lauren
Lavina
Leigh
Leigha
Leslie
Lindell
Lindsay
Lynn
Mackenna
Madison
Makaila
Makenna
Makenzey
Malka
Marina
Mary
Maureen
Maya
Megan
Melanie
Melissa
Michaela
Mikenna
Miranda
Morgan
Nada
Naomi
Natalie
Nathalie
Nicole
Nuala
Paisley
Paris
Payton
Peyton
Persephone
Rachel
Raelyn
Rebecca
Reece
Reilly
Renee
Riann
Rhianna
Robin
Ruby
Rupali
Sabrina
Sapphyre
Sarah
Sari
Scarlet
Secret
Shanequa
Shannon
Sharon
Shenita
Shiquita
Sierra
Skylar
Sloane
Solace
Sophie
Talia
Tamara
Tanaki
Tanika
Tayleigh
Taylor
Trinity
Tess
Tessa
Theresa
Timothie
Trinity
Vanea
Vanessa
Vanya
Vedika
Veronica
Vicki
Victoria
Whitley
Whitney
Xenia
Xzyliah
Zagros
Zita
Zoe
=Last
ABBOTT
ACOSTA
ADAMS
ADKINS
AGUILAR
ALEXANDER
ALLEN
ALLISON
ALVARADO
ALVAREZ
ANDERSON
ANDREWS
ANTHONY
ARMSTRONG
ARNOLD
ASHLEY
ATKINS
ATKINSON
AUSTIN
AVERY
AVILA
AYALA
AYERS
BAILEY
BAKER
BALDWIN
BALL
BALLARD
BANKS
BARBER
BARKER
BARNES
BARNETT
BARR
BARRETT
BARRON
BARRY
BARTLETT
BARTON
BASS
BATES
BAUER
BAXTER
BEAN
BEARD
BEASLEY
BECK
BECKER
BELL
BENNETT
BENSON
BENTLEY
BENTON
BERG
BERRY
BEST
BISHOP
BLACK
BLACKBURN
BLACKWELL
BLAIR
BLAKE
BLANCHARD
BLANKENSHIP
BLEVINS
BOND
BOOKER
BOONE
BOOTH
BOWEN
BOWERS
BOWMAN
BOYD
BOYER
BRADFORD
BRADLEY
BRADSHAW
BRADY
BRANCH
BRENNAN
BREWER
BRIDGES
BRIGGS
BROCK
BROOKS
BROWN
BROWNING
BRUCE
BRYAN
BRYANT
BUCHANAN
BUCK
BUCKLEY
BULLOCK
BURGESS
BURKE
BURNETT
BURNS
BURTON
BUSH
BUTLER
BYRD
CAIN
CALDWELL
CALHOUN
CALLAHAN
CAMACHO
CAMERON
CAMPBELL
CAMPOS
CANNON
CANTU
CARDENAS
CAREY
CARLSON
CARPENTER
CARR
CARRILLO
CARROLL
CARSON
CARTER
CASEY
CASTILLO
CASTRO
CHAMBERS
CHAN
CHANDLER
CHANG
CHAPMAN
CHARLES
CHASE
CHAVEZ
CHEN
CHERRY
CHRISTENSEN
CHRISTIAN
CHURCH
CLARK
CLARKE
CLAY
CLAYTON
CLINE
COBB
COCHRAN
COHEN
COLE
COLEMAN
COLLIER
COLLINS
COLON
COMBS
CONLEY
CONNER
CONTRERAS
CONWAY
COOK
COOPER
COPELAND
CORTEZ
COX
CRAIG
CRAWFORD
CROSBY
CROSS
CRUZ
CUMMINGS
CUNNINGHAM
CURRY
CURTIS
DALTON
DANIEL
DANIELS
DAVENPORT
DAVIDSON
DAVIS
DAWSON
DAY
DEAN
DECKER
DELEON
DELGADO
DENNIS
DIAZ
DICKERSON
DICKSON
DIXON
DODSON
DOMINGUEZ
DONALDSON
DORSEY
DOTSON
DOUGLAS
DOYLE
DRAKE
DUDLEY
DUNCAN
DUNN
DURAN
DURHAM
DYER
EATON
EDWARDS
ELLIOTT
ELLIS
ELLISON
ENGLISH
ERICKSON
ESPINOZA
ESTES
ESTRADA
EVANS
EVERETT
FARMER
FARRELL
FAULKNER
FERGUSON
FERNANDEZ
FIELDS
FIGUEROA
FINLEY
FISCHER
FISHER
FITZGERALD
FLEMING
FLETCHER
FLORES
FLOWERS
FLOYD
FLYNN
FOLEY
FORD
FOSTER
FOWLER
FOX
FRANCIS
FRANK
FRANKLIN
FRAZIER
FREEMAN
FRENCH
FROST
FUENTES
FULLER
GAINES
GALLAGHER
GALLEGOS
GAMBLE
GARCIA
GARDNER
GARNER
GARRETT
GARRISON
GARZA
GATES
GAY
GENTRY
GEORGE
GIBBS
GIBSON
GILBERT
GILES
GILL
GILLESPIE
GILMORE
GLASS
GLENN
GLOVER
GOLDEN
GOMEZ
GONZALES
GONZALEZ
GOODMAN
GOODWIN
GORDON
GRAHAM
GRANT
GRAVES
GRAY
GREEN
GREENE
GREER
GREGORY
GRIFFIN
GRIFFITH
GRIMES
GROSS
GUERRA
GUERRERO
GUTIERREZ
GUZMAN
HALE
HALEY
HALL
HAMILTON
HAMMOND
HAMPTON
HANCOCK
HANSEN
HANSON
HARDIN
HARDING
HARDY
HARMON
HARPER
HARRELL
HARRINGTON
HARRIS
HARRISON
HART
HARTMAN
HARVEY
HATFIELD
HAWKINS
HAYES
HAYNES
HEATH
HEBERT
HENDERSON
HENDRICKS
HENRY
HENSLEY
HENSON
HERMAN
HERNANDEZ
HERRERA
HERRING
HESS
HICKMAN
HICKS
HIGGINS
HILL
HINES
HOBBS
HODGE
HODGES
HOFFMAN
HOGAN
HOLLAND
HOLLOWAY
HOLMAN
HOLMES
HOLT
HOOD
HOOVER
HOPKINS
HORN
HORNE
HORTON
HOUSE
HOUSTON
HOWARD
HOWELL
HUBBARD
HUDSON
HUFF
HUFFMAN
HUGHES
HULL
HUMPHREY
HUNT
HUNTER
HURST
HUTCHINSON
INGRAM
JACKSON
JACOBS
JACOBSON
JAMES
JEFFERSON
JENKINS
JENNINGS
JENSEN
JIMENEZ
JOHNS
JOHNSON
JOHNSTON
JONES
JORDAN
JOSEPH
JOYCE
JUAREZ
KANE
KEITH
KELLER
KELLEY
KELLY
KEMP
KENNEDY
KENT
KERR
KEY
KIM
KING
KIRBY
KIRK
KLEIN
KLINE
KNAPP
KNIGHT
KNOX
KOCH
KRAMER
LAMB
LAMBERT
LANDRY
LANE
LANG
LARA
LARSEN
LARSON
LAWRENCE
LAWSON
LEACH
LEBLANC
LEE
LEON
LEONARD
LESTER
LEWIS
LINDSEY
LITTLE
LIVINGSTON
LLOYD
LOGAN
LONG
LOPEZ
LOVE
LOWE
LOWERY
LUCAS
LUNA
LYNCH
LYONS
MACK
MADDOX
MALDONADO
MALONE
MANN
MANNING
MARKS
MARQUEZ
MARSH
MARSHALL
MARTIN
MARTINEZ
MASON
MASSEY
MATHEWS
MATHIS
MATTHEWS
MAXWELL
MAY
MAYNARD
MAYS
MCBRIDE
MCCALL
MCCARTHY
MCCLAIN
MCCLURE
MCCONNELL
MCCORMICK
MCCOY
MCCULLOUGH
MCDANIEL
MCDONALD
MCDOWELL
MCGEE
MCGUIRE
MCINTOSH
MCINTYRE
MCKEE
MCKENZIE
MCKINNEY
MCKNIGHT
MCLAUGHLIN
MCLEAN
MCMILLAN
MEADOWS
MEDINA
MEJIA
MELTON
MENDEZ
MENDOZA
MERCADO
MERRITT
MEYER
MEYERS
MIDDLETON
MILES
MILLER
MILLS
MIRANDA
MITCHELL
MOLINA
MONROE
MONTGOMERY
MONTOYA
MOODY
MOON
MOORE
MORALES
MORAN
MORENO
MORGAN
MORIN
MORRIS
MORRISON
MORROW
MORSE
MORTON
MOSES
MOSLEY
MOSS
MUELLER
MULLINS
MUNOZ
MURPHY
MURRAY
MYERS
NASH
NAVARRO
NEAL
NELSON
NEWMAN
NEWTON
NGUYEN
NICHOLS
NICHOLSON
NIXON
NOBLE
NOLAN
NORMAN
NORRIS
NORTON
NUNEZ
OBRIEN
OCHOA
OCONNOR
OLIVER
OLSEN
OLSON
ONEAL
ONEILL
ORR
ORTEGA
ORTIZ
OSBORNE
OWEN
OWENS
PACE
PACHECO
PADILLA
PAGE
PALMER
PARK
PARKER
PARKS
PARRISH
PARSONS
PATEL
PATRICK
PATTERSON
PATTON
PAUL
PAYNE
PEARSON
PECK
PENA
PENNINGTON
PEREZ
PERKINS
PERRY
PETERS
PETERSEN
PETERSON
PHELPS
PHILLIPS
PIERCE
PITTMAN
PITTS
POOLE
POPE
PORTER
POTTER
POWELL
POWERS
PRATT
PRESTON
PRICE
PRINCE
PRUITT
PUGH
QUINN
RAMIREZ
RAMOS
RAMSEY
RANDALL
RANDOLPH
RAY
REED
REESE
REEVES
REID
REYES
REYNOLDS
RHODES
RICE
RICH
RICHARD
RICHARDS
RICHARDSON
RICHMOND
RILEY
RIOS
RIVAS
RIVERA
RIVERS
ROACH
ROBBINS
ROBERSON
ROBERTS
ROBERTSON
ROBINSON
ROBLES
RODGERS
RODRIGUEZ
RODRIQUEZ
ROGERS
ROJAS
ROLLINS
ROMAN
ROMERO
ROSE
ROSS
ROTH
ROWE
ROY
RUIZ
RUSH
RUSSELL
RUSSO
RYAN
SALAS
SALAZAR
SALINAS
SANCHEZ
SANDERS
SANDOVAL
SANFORD
SANTIAGO
SANTOS
SAUNDERS
SAVAGE
SAWYER
SCHMIDT
SCHNEIDER
SCHROEDER
SCHULTZ
SCHWARTZ
SCOTT
SELLERS
SERRANO
SEXTON
SHAFFER
SHANNON
SHARP
SHAW
SHELTON
SHEPARD
SHEPHERD
SHEPPARD
SHERMAN
SHIELDS
SHORT
SILVA
SIMMONS
SIMON
SIMPSON
SIMS
SINGLETON
SKINNER
SLOAN
SMALL
SMITH
SNOW
SNYDER
SOLIS
SOLOMON
SOTO
SPARKS
SPEARS
SPENCER
STAFFORD
STANLEY
STEELE
STEPHENS
STEPHENSON
STEVENS
STEVENSON
STEWART
STOKES
STONE
STRICKLAND
STRONG
SULLIVAN
SUMMERS
SUTTON
SWANSON
SWEENEY
TANNER
TATE
TAYLOR
TERRELL
TERRY
THOMAS
THOMPSON
THORNTON
TILLMAN
TODD
TORRES
TOWNSEND
TRAN
TREVINO
TRUJILLO
TUCKER
TURNER
TYLER
UNDERWOOD
VALDEZ
VALENTINE
VANCE
VARGAS
VASQUEZ
VAUGHN
VAZQUEZ
VEGA
VELASQUEZ
VELEZ
VILLARREAL
VINCENT
WADE
WAGNER
WALKER
WALL
WALLACE
WALLER
WALLS
WALSH
WALTER
WALTERS
WALTON
WARD
WARE
WARNER
WARREN
WASHINGTON
WATERS
WATKINS
WATSON
WATTS
WEAVER
WEBB
WEBER
WEBSTER
WEEKS
WEISS
WELCH
WELLS
WEST
WHEELER
WHITAKER
WHITE
WHITEHEAD
WHITNEY
WIGGINS
WILCOX
WILEY
WILKERSON
WILKINS
WILKINSON
WILLIAM
WILLIAMS
WILLIAMSON
WILLIS
WILSON
WINTERS
WISE
WOLF
WOLFE
WONG
WOOD
WOODARD
WOODS
WOODWARD
WRIGHT
WYATT
YATES
YORK
YOUNG
ZIMMERMAN
