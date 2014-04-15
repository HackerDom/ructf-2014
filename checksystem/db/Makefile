help:
	echo Targets: main-db test-db clean

main-db:
	psql -f game-databases-create.sql
	./run.sh ructf2014 ructf2014 tables-create.sql
	./run.sh ructf2014 ructf2014 ructf2014-services.sql
	./run.sh ructf2014 ructf2014 game-teams-add.sql
	./change-owner.sh ructf2014 ructf2014

test-db:
	psql -f test-databases-create.sql
	./run.sh testuser ructfetest tables-create.sql
	./run.sh testuser ructfetest ructf2014-services.sql
	./run.sh testuser ructfetest test-teams-add.sql
	./change-owner.sh ructfetest testuser

clean:
	./run.sh tables-clear.sql
