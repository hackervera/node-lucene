var lucene = require("./build/default/lucene_bindings.node");
var Lucene = new lucene.Lucene();



//Index key value pairs
var people = [{name: "tyler", place_of_birth: "portland city of roses"}];
for(i=0;i<people.length;i++){
	var person = people[i];
	for(var key in person){
		Lucene.indexText(key,person[key],"indexes/"+person.name);
	};
	var search_term = "portland";
	var results = Lucene.search("indexes/"+person.name, search_term);
	for(j=0;j<results.length;j++){
		var result = results[j].split(" ");
		console.log("Found {\""+result[0]+"\":\""+person[result[0]]+"\"} with score of "+result[1]+" for search of '"+search_term+"'"); 
	}


};

/* Expected output:

/Users/tyler/node-lucene> node lucene.js    
Indexing took: 4 ms.

Indexing took: 8 ms.

Searching for: portland

0. place_of_birth - 0.500000


Search took: 0 ms.
Screen dump took: 0 ms.

Found {"place_of_birth":"portland city of roses"} with score of 0.500000 for search of 'portland'

*/



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
