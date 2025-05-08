## Groups ##

This is a feature designed to run groups of tap tests with arbitrary config.
The group config can be altered by pre- and post- scripts.

- groups are defined by creating a folder in `groups` and placing configuration scripts inside
- group membership for tap tests is defined in `groups.json`
- group naming is arbitrary, can be descriptive
  - `scenario_config`
  - `mysql-query_digests=0`
- to increase testing concurency split group into multiple groups and add a `-gX` appendix
  - `group_name-g1`
  - `group_name-g2`
  - `group_name-g3`
- configs are applied via scripts placed inside respective `group_name` folder
  - `pre-*.bash` script run before tests
  - `pre-*.sql` proxysql config run before tests
  - `post-*.bash` script run after tests
  - `post-*.sql` proxysql config run after tests
- configuration scripts are executed in alphabetical order


example test group `mytests-g1` can be created by
```
TG='mytests-g1'
mkdir -p test/tap/groups/$TG
cd test/tap/groups/$TG
cat > pre-proxysql.sql << EOF
# run this test group with:
SET mysql-multiplexing='false';
LOAD MYSQL VARIABLES TO RUNTIME;
SAVE MYSQL VARIABLES TO DISK;
EOF
```
also tap test group membership needs to be updated in `groups.json` e.g.:

```
{
 "test1-t" : ["group1", "group2", "group3"],
 "test2-t" : ["group2", "group3"],
 "test3-t" : ["group1"],
 "test4-t" : [],
 "test5-t" : ["group1", "group3", "group4"]
}
```

