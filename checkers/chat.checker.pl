#!/usr/bin/perl
use strict;
use IO::Socket::INET;
++$|;

######## Config ########

my $TIMEOUT     = 5;
my $DEBUG       = 1;
my $PORT        = 5555;
my $SLEEP_TIME  = 0.4;

########################

sub SLEEP        { select(undef,undef,undef,$SLEEP_TIME) }

sub MSG          { my $s = shift; $s =~ s/[\r\n]+/[\\n]/g; $s; }

sub EXIT_OK      { print MSG(pop).$/; exit 101 }
sub EXIT_CORRUPT { print MSG(pop).$/; exit 102 }
sub EXIT_MUMBLE  { print MSG(pop).$/; exit 103 }
sub EXIT_DOWN    { print MSG(pop).$/; exit 104 }
sub EXIT_ERR     { print MSG(pop).$/; exit 105 }

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
#   debug "read_data: First=%d, Last=%d", 0+@First, 0+@Last;
}

sub check {
    my ($host) = @_;
    my $s = IO::Socket::INET->new(
        PeerAddr => $host,
        PeerPort => $PORT,
        Proto    => 'tcp',
        Timeout  => $TIMEOUT
    ) or EXIT_DOWN "Service is down: $@";
    $s->recv(my $data, 1024);
    $s->close();
    $data =~ /::: Welcome to RuCTF 2014 passenger chat :::/ ? EXIT_OK : EXIT_MUMBLE;
}

sub put {
    my ($host, $id, $flag) = @_;
    $id =~ s/-//g;

    read_data();

    my $login = sprintf "%s.%s", $First[int rand @First], $Last[int rand @Last];

    my $room  = substr $id, 0, 6;
    my $pass  = substr $id, 6;

    debug "put: generated: $login, $room, $pass";

    my $s = IO::Socket::INET->new(
        PeerAddr => $host,
        PeerPort => $PORT,
        Proto    => 'tcp',
        Timeout  => $TIMEOUT
    ) or EXIT_DOWN "Service is down: $@";

    SLEEP();
    $s->recv(my $data, 2048);

    $s->send("\\register $login $pass\r\n");
    SLEEP();
    $s->recv(my $data, 2048);
    $data =~ /Login OK/ or EXIT_MUMBLE "register failed: '$data'";

    debug "register OK";

    $s->send("\\create $room $pass\r\n");
    SLEEP();
    $s->recv(my $data, 1024);
    $data =~ /Welcome to private room '$room'/ or EXIT_MUMBLE "create room failed: '$data'";

    debug "create room OK";

    $s->send("Hello, world!\r\n");
    SLEEP();
    $s->recv(my $data, 1024);

    debug "say hello OK";

    $s->send("$flag\r\n");
    SLEEP();
    $s->recv(my $data, 1024);

    $data =~ /$flag/ or EXIT_MUMBLE "say flag failed";

    debug "say flag OK";

    $s->send("\\quit\r\n");

    EXIT_OK join "_", $login, $room, $pass;
}

sub get {
    my ($host, $id, $flag) = @_;

    my ($login, $room, $pass) = split '_', $id;

    my $s = IO::Socket::INET->new(
        PeerAddr => $host,
        PeerPort => $PORT,
        Proto    => 'tcp',
        Timeout  => $TIMEOUT
    ) or EXIT_DOWN "Service is down: $@";

    SLEEP();
    $s->recv(my $data, 2048);

    $s->send("\\login $login $pass\r\n");
    SLEEP();
    $s->recv(my $data, 2048);
    $data =~ /Login OK/ or EXIT_MUMBLE "login failed: '$data'";

    debug "login OK";

    $s->send("\\join $room $pass\r\n");
    SLEEP();
    $s->recv(my $data, 1024);
    $data =~ /Welcome to private room '$room'/ or EXIT_MUMBLE "join room failed: '$data'";

    debug "join room OK";

    $data =~ /$flag/ or EXIT_MUMBLE "flag not found: '$data'";

    EXIT_OK "flag check OK";
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
ABBEY
ABEL
ABNEY
ABRAHAM
ABRAHAMS
ABRAHAMSON
ABRAM
ABRAMS
ABRAMSON
ACHILLES
ACKER
ACKERMAN
ACKERMAN
ADAIR
ADAM
ADAMS
ADAMSON
ADCOCK
ADDISON
ADKINS
AIKEN
AINSWORTH
AITKEN
AKERMAN
AKERMAN
AKERS
ALBERT
ALBERTS
ALBERTSON
ALBINSON
ALDEN
ALEXANDER
ALFREDSON
ALFSON
ALGER
ALLARD
ALLEN
ALLSOPP
ALVEY
ALVIN
ANDERSON
ANDREWS
ANDREWSON
ANSEL
ANSON
ANTHONYSON
APPLEBY
APPLETON
ARCHER
ARKWRIGHT
ARMISTEAD
ARNOLD
ARRINGTON
ARTERBERRY
ARTERBURY
ARTHUR
ARTHURSON
ASH
ASHLEY
ASHWORTH
ATKINS
ATKINSON
ATTAWAY
ATTEBERRY
ATTERBERRY
ATTWATER
AUDLEY
AUGUSTINE
AUSTIN
AUTEBERRY
AUTENBERRY
AUTTENBERG
AVERILL
AVERY
AYERS
AYERS
AYERS
AYLMER
AYTON
BABCOCK
BABCOCKE
BABCOKE
BACKUS
BADCOCK
BADCOCKE
BADCOKE
BAGLEY
BAILEY
BAINES
BAKER
BALDWIN
BANCROFT
BANISTER
BANKS
BANNER
BANNERMAN
BARBER
BARDSLEY
BARKER
BARLOW
BARNES
BARRET
BARRETT
BARTON
BARTRAM
BASS
BATES
BATESON
BATTLE
BATTS
BAXTER
BEAKE
BEASLEY
BEATTIE
BECK
BECK
BECK
BECKET
BECKETT
BECKHAM
BELCHER
BELL
BELL
BELLAMY
BENBOW
BENJAMINSON
BENNET
BENNETT
BENSON
BENTLEY
BENTON
BERNARD
BERRY
BEVERLEY
BEVERLY
BIRD
BISHOP
BLACK
BLACKBOURNE
BLACKBURN
BLACKMAN
BLACKWOOD
BLAKE
BLAKESLEE
BLOODWORTH
BLOXAM
BLOXHAM
BLUE
BLYTHE
BOIVIN
BOLTON
BOND
BONE
BONE
BONHAM
BONHER
BONNER
BONNEY
BOON
BOON
BOONE
BOONER
BOOTHMAN
BOTWRIGHT
BOURKE
BOYCE
BRADDOCK
BRADFORD
BRADLEY
BRAMS
BRAMSON
BRAND
BRANDON
BRANT
BRASHER
BRASSINGTON
BRAY
BRECKENRIDGE
BRECKINRIDGE
BRENT
BREWER
BREWSTER
BRIGHAM
BRISTOL
BRISTOW
BRITTON
BROADBENT
BROCK
BROOK
BROOKE
BROOKS
BROWN
BROWNLOW
BRYAN
BRYANT
BRYSON
BUCKLEY
BUCKLEY
BULLARD
BULLE
BULLOCK
BUNKER
BURKE
BURNHAM
BURNS
BURRELL
BURTON
BUSH
BUTCHER
BUTLER
BUTTS
BYRD
CANNON
CANTRELL
CARL
CARLISLE
CARLYLE
CARMAN
CARMAN
CARPENTER
CARTER
CARTWRIGHT
CARVER
CAULFIELD
CAUSER
CAUSEY
CHADWICK
CHAMBERLAIN
CHANCE
CHANCELLOR
CHANDLER
CHAPMAN
CHASE
CHESHIRE
CHRISTIANS
CHRISTIANSON
CHRISTINSEN
CHRISTISON
CHRISTOPHER
CHRISTOPHERS
CHRISTOPHERSON
CHURCH
CLARK
CLARKE
CLARKSON
CLAY
CLAYTON
CLEMENS
CLIFFORD
CLIFTON
CLINE
CLINTON
CLOSE
COBURN
COCK
COCKBURN
COCKS
COEL
COKE
COKES
COKES
COLBERT
COLE
COLEMAN
COLLINGWOOD
COLLINS
COLTON
COMBS
COMSTOCK
CONSTABLE
COOK
COOKE
COOKSON
COOMBS
COOPER
COREY
CORNELL
CORRA
CORY
COTTERILL
COURTENAY
COURTENAY
COURTNEY
COWDEN
COX
CRAWFORD
CREWE
CROFT
CROPPER
CROSS
CROUCH
CUMMINS
CURTIS
DABNEY
DALTON
DANE
DANELL
DANIEL
DANIELL
