var lucene = require ("./build/default/lucene_bindings.node");
var Lucene = new lucene.Lucene();
Lucene.index("/users/tyler/texts","/users/tyler/indexes");
Lucene.search("/users/tyler/indexes", "test");
