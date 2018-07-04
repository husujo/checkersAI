

var socket = io();

var starting_board = [	[ 0, -1, 0, -1, 0, -1, 0, -1],
						[ -1, 0, -1, 0, -1, 0, -1, 0],
						[ 0, -1, 0, -1, 0, -1, 0, -1],
						[ 0, 0, 0, 0, 0, 0, 0, 0],
						[ 0, 0, 0, 0, 0, 0, 0, 0],
						[ 1, 0, 1, 0, 1, 0, 1, 0],
						[ 0, 1, 0, 1, 0, 1, 0, 1],
						[ 1, 0, 1, 0, 1, 0, 1, 0]]
var current_board = starting_board


var turn = 1;
var move = "";

var selected = [-69,-69]

window.onload = function() {
	document.getElementById("myCanvas").addEventListener("mousedown", getPosition, false);
	drawBoard()
	console.log("loaded!");
}

socket.on('computer move', function(data) {
	// make the computer move
	console.log("ai move: "+data);
	makeComputerMove(data)
})

socket.on('gameover', function() {
	console.log("gameover");
})

function getPosition(event) {
	var x = event.x;
	var y = event.y;
	var canvas = document.getElementById("myCanvas");
	x -= canvas.offsetLeft;
	y -= canvas.offsetTop;
	// console.log("x:" + x + " y:" + y);

	// get the board coords of the selection
	// set the real board choords
	var boardCol = Math.floor(x/50)-1;
	var boardRow = 8-Math.floor(y/50);
	// console.log("Col:" + boardCol + " Row:" +boardRow)

	// if haven't selected a piece yet
	if (selected[0] == -69) {
		// console.log("try to select");
		if (current_board[7-boardRow][boardCol] <= 0) {
			console.log("please select a valid piece");
			return; // not a selection
		}
		// set selected
		selected[0] = boardRow;
		selected[1] = boardCol;
		move = "" + selected[1] + selected[0]; // reset move

	} else { // already selected, try to move there, or continue jumping
		// console.log(current_board[selected[0]][selected[1]]);
		// console.log("selected square: " + selected[0] + "  " +  selected[1]);


		// check if the space is adjacent to the previous one, if not, it's a jump
		var dist = Math.abs(selected[0]-boardRow);
		var jump;
		if (dist == 2) { // it's a jump!
			jump = true;
		} else {
			jump = false;
		}

		var takes = jump?"xx":"->"


		move += takes + boardCol + boardRow
		selected[0] = boardRow // use for recalculating the piece position, and if it's a jump
		// console.log("checking move " + move)

		// try to make the move. on failure, its an incorrect move. on keep jumping, must keep jumping. otherwise the move will be made!
		makePlayerMove(move)

	}
}

function drawBoard() {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");

	// clear the canvas
	ctx.clearRect(0,0,500,500);

	// var img = document.getElementById("myimg");
	// ctx.drawImage(img,10,10);

	// draw board:
	var color=1;
	for (var i = 0; i < 8; i++) {
		for (var j = 0; j < 8; j++) {
			// if (color) { // white
			// 	// ctx.fillStyle = '#000000';
			// 	// ctx.fillRect(50*i,50*j,50,50);
			// }
			if (!color) {
				ctx.fillStyle = '#996633';
				ctx.fillRect(50*i+50,50*j+50,50,50);
			}
			color = !color;
		}
		color = !color
	}
	ctx.font = "30px Arial";

	var row=7;
	for (var i = 0; i < 8; i++) {
		ctx.fillText(i,65+50*i,35)
		ctx.fillText(i,65+50*i,35+450)

		ctx.fillText(row,15,85+50*i)
		ctx.fillText(row,15+450,85+50*i)
		row--;
	}

	// the pieces

	for (var i = 0; i < 8; i++) {
		for (var j = 0; j < 8; j++) {
			if (current_board[i][j] != 0) {
				// console.log(current_board[i][j].toString())
				// if (current_board[i][j] == 1) {
					// ctx.fillCircle(50*(j+1),50*(i+1),50,50);
				// } else {
					ctx.drawImage(document.getElementById(current_board[i][j].toString()),50*(j+1),50*(i+1),50,50);
				// }
			}
		};
	};
}

// TODO make longer, for many jumps
function applyMove(move) {
	move = move.trim();
	// move = d2->d4 = column,row,column,row
	// move = 33xx55xx77
	var start_col = move.charCodeAt(0)-48;
	var start_row = move.charCodeAt(1) - 48;
	var jump = (move.charAt(2)=='x')?true:false;
	var end_col = move.charCodeAt(4) - 48;
	var end_row = move.charCodeAt(5) -48;

	// console.log("applying move starting from " + start_col + start_row)

	var temp = current_board[7-start_row][start_col];
	current_board[7-start_row][start_col] = 0; // test this
	
	if (jump) {
		var index = 4;
		// console.log("move.length:"+move.length);
		var numJumps = (move.length-2)/4; // the number of jumps made
		var row,file,drow,dfile,rrow,rfile;
		row = start_row;
		file = start_col;
		// console.log("number of jumps=" + numJumps);
		for (var i=0; i<numJumps; i++) {
			drow = move.charCodeAt(index+1)-48;
			dfile = move.charCodeAt(index)-48;
			// console.log("dfile,drow:" + dfile + "" + drow);

			rrow = (row+drow)/2;
			rfile = (file+dfile)/2;

			// console.log("removing piece at " + rfile + "" + rrow);
			current_board[7-rrow][rfile] = 0;

			row = drow;
			file = dfile;
			index+=4;
		}

		// current_board[7-drow][dfile] = temp;

		end_row=drow;
		end_col=dfile;

		
	}

	if (temp==1&&end_row==7) {
		console.log("king me!");
		temp=2;
	} else if (temp==-1&&end_row==0) {
		temp=-2;
		console.log("king me!");
	}

	current_board[7-end_row][end_col] = temp;

}

function makePlayerMove(move) {
	if (turn != 1) {
		console.log("not your turn, player")
		move = ""
		selected[0] = -69;
		selected[1] = -69;
		return
	}

	socket.emit('player move', move);

}

var moveToMake;

socket.on('valid move', function() {
	// console.log("valid move")
	// redraw board to show player's move:

	applyMove(move) // re-set current_board in here
	drawBoard()
	turn = -1;
	console.log("you moved " + move)
	selected[0] = -69;
	selected[1] = -69;
	move = ""
})


socket.on('invalid move',function() {
	move="";
	console.log("invalid move, make a different move");
	selected[0] = -69;
	selected[1] = -69;
})

socket.on('keep jumping', function() {
	console.log("keep jumping!!");
})


function makeComputerMove(move) {
	if (turn != -1) {
		console.log("not your turn, computer")
		return
	}
	// redraw board to show player's move:
	applyMove(move) // re-set current_board in here
	drawBoard()
	turn = 1;
	// console.log(move)
}










