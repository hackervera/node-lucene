var lucene = require ("./build/default/lucene_bindings.node");
var Lucene = new lucene.Lucene();
//console.log(Lucene.hello());
//Lucene.index("/users/tyler/texts","/users/tyler/indexes");
console.log(Lucene.search("/users/tyler/indexes", "test"));
