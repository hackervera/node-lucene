var lucene = require ("./build/default/lucene_bindings.node");
var Lucene = new lucene.Lucene();
//console.log(Lucene.hello());
Lucene.index("/users/tyler/texts","/users/tyler/indexes");
Lucene.indexText("example twitter account","tylergillies is awesome","/users/tyler/indexes")
Lucene.indexText("another example twitter account","lockerproject is awesome","/users/tyler/indexes")
console.log(Lucene.search("/users/tyler/indexes", "tylergillies"));
console.log(Lucene.search("/users/tyler/indexes", "lockerproject"));
console.log(Lucene.search("/users/tyler/indexes", "pet"));
