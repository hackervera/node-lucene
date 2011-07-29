var lucene = require("./build/default/lucene_bindings.node");
var Lucene = new lucene.Lucene();



//Index key value pairs
var people = [{name: "tyler", place_of_birth: "portland, city of roses"}];
for(i=0;i<people.length;i++){
	person = people[i];
	for(var key in person){
		Lucene.indexText(key,person[key],"indexes/"+person.name);
	};
	var search_term = "portland";
	var result = Lucene.search("indexes/"+person.name, search_term);
	console.log("Found "+key+":"+person[key]+" for search of '"+search_term+"'"); 
};



/*
Lucene.indexText("name","tylergillies","indexes/tyler")
Lucene.indexText("place_of_birth","portland","indexes/tyler")


Index a directory of files
Lucene.index("/users/tyler/texts","indexes");
console.log(Lucene.search("indexes", "tylergillies"));

console.log(Lucene.search("/users/tyler/indexes", "lockerproject"));


Lucene.search("indexes", "pet")
console.log(Lucene.search("indexes", "portland"));
*/
