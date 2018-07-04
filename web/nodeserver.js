

var express = require('express')
var app = express()
var http = require('http')
var server = http.createServer(app);
var io = require('socket.io').listen(server)


var fs = require('fs')
// var exec = require('child_process').exec;
var spawn = require('child_process').spawn

// app.use(express.static(path.join(__dirname,'public')));
app.use(express.static(__dirname))


io.on('connection', function(socket) {

	var ai = spawn('./ai/client',['web'])

	//console.log('a user connected');

	ai.stdout.on('data', function(data){ // getting the computer's move

		var str = data.toString();
		// console.log(data)
		if (data=="gameover\n") {
			//console.log("gameover")
			// socket.emit("valid move")
			socket.emit('gameover')
			// setTimeout(function() {socket.emit('gameover', data.toString())}, 500)
		} else if (data=="invalid move\n") {
			//console.log("invalid move")
			socket.emit('invalid move')
		} else if (data=="keep jumping\n") {
			//console.log("keep jumping")
			socket.emit('keep jumping')
		} else if (data=="valid move\n") {
			//console.log("valid move");
			socket.emit('valid move');
		} else if (str.split(" ")[0] == "DEBUG") {

			console.log(str);

		} else {
			// console.log(data);
			console.log("ai move:\t" + data.toString())

			// dont want the move to be instantaneous so the user doenst see it
			setTimeout(function() {socket.emit('computer move', data.toString())}, 200) 

		}
		// console.log("getting data from ai")
	 // 	console.log(data.toString())

	 	
	})

	ai.stdout.on('end', function(){
	 	console.log('end of ai')
	 	socket.emit("gameover");
	})
	
	socket.on('disconnect', function(){
		//console.log('user disconnected');
	});
	socket.on('player move', function(move){
    	console.log('player move: ' + move);

    	// send the ai the player's move
    	ai.stdin.write(move)
    	ai.stdin.write("\n")

    	// ai.stdin.end()
  	});
});

app.get('/', function (req, res) {

	// fs.readFile('position.txt', 'utf8', function (err,data) {
	// 	if (err) { return console.log(err) }
	// 	console.log(data);
	// });

	res.sendFile('checkers.html', {root: __dirname })
})



server.listen(3000, function () {
  console.log('server listening on port 3000!')
})








