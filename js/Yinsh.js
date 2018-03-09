var dimension = [document.documentElement.clientWidth, document.documentElement.clientHeight];
var game_canvas = document.getElementById('GameBoard');

game_canvas.width = dimension[1]*0.75;
game_canvas.height = dimension[1]*0.75;

var piece_canvas = document.getElementById('PieceLayer');

piece_canvas.width = dimension[1]*0.75;
piece_canvas.height = dimension[1]*0.75;

var centerx = game_canvas.width/2;
var centery = game_canvas.width/2;

var spacing = game_canvas.height/11;
var altitude=spacing*Math.sqrt(3)/2;

var game_ctx = game_canvas.getContext("2d");

function Point(x, y) {
  this.x = x;
  this.y = y;
}

var positions = new Array(11);
for (var i = 0; i < 11; i++) {
  positions[i] = new Array(11);
}

function LineTest(){
	game_ctx.beginPath();
	//game_ctx.moveTo(positions[i][begin].x,positions[i][begin].y);
	//game_ctx.lineTo(positions[i][end].x,positions[i][end].y);
	game_ctx.moveTo(0,0);
	game_ctx.lineTo(100,100);
	game_ctx.stroke();
}

function PlotPoints(){
	for (var i = 0; i < 11; i++) 
	{
		

		var x=i-5;
		var low=-5;
		var high=5;
		if(x==0){
			low=-4;
			high=4;
		}
		if(x>=1&&x<=4){
			low=-5+x;
		}
		if(x==5){
			low=1;
			high=4;
		}
		if(x>=-4&&x<=-1){
			high=5+x;
		}
		if(x==-5){
			low=-4;
			high=-1;
		}

		for(var j=0;j<11;j++){
			var y=j-5;
			
			if(!(y>=low&&y<=high)){
				positions[i][j]= new Point(-1,-1);
				continue;
			}
			
			positions[i][j]= new Point(centerx+altitude*x,centery-spacing*(y-x/2));
		}
	}
	
}

function DrawBoardLines(){
	for(var i=0;i<11;i++){
		var begin=0;
		var end=10;
		var j=0;

		while(positions[i][j].x==-1&&j<11){
			j++;
			if(j==11){
				break;
			}
		}
		begin=j;
		while(positions[i][j].x!=-1&&j<11){
			j++;
			if(j==11){
				break;
			}
		}
		end=j-1;
		game_ctx.beginPath();
		game_ctx.moveTo(positions[i][begin].x,positions[i][begin].y);
		game_ctx.lineTo(positions[i][end].x,positions[i][end].y);
		//game_ctx.moveTo(0,0);
		//game_ctx.lineTo(100,100);
		game_ctx.stroke();
		
	}
	for(var j=0;j<11;j++){
		var begin=0;
		var end=10;
		var i=0;

		while(positions[i][j].x==-1&&i<11){
			i++;
			if(i==11){
				break;
			}
		}
		begin=i;
		while(positions[i][j].x!=-1&&i<11){
			i++;
			if(i==11){
				break;
			}
		}
		end=i-1;
		game_ctx.beginPath();
		game_ctx.moveTo(positions[begin][j].x,positions[begin][j].y);
		game_ctx.lineTo(positions[end][j].x,positions[end][j].y);
		//ctx.moveTo(0,0);
		//ctx.lineTo(100,100);
		game_ctx.stroke();
		
	}

	for(var i=0;i<11;i++){
		for(var j=0;j<11;j++){
			var x=i-5;
			var y=j-5;
			if(Math.abs(x)==5||(x==-4&&y==-4)||(x==-4&&y==1)||(x==1&&y==-4)){
				game_ctx.beginPath();
				game_ctx.moveTo(positions[i][j].x,positions[i][j].y);
				game_ctx.lineTo(positions[5-y][5-x].x,positions[5-y][5-x].y);
				//game_ctx.moveTo(0,0);
				//game_ctx.lineTo(100,100);
				game_ctx.stroke();
			}

		}
	}
}

function ColorItUp(){

	for(var i=0;i<11;i++){
		for(var j=0;j<11;j++){

			if(positions[i][j].x==-1){
				continue;
			}
			ctx.beginPath();
			if(i==0){
				game_ctx.strokeStyle="#FF0000";
			}
			else if(i==1){
				game_ctx.strokeStyle="#00FF00";
			}
			else if(i==2){
				game_ctx.strokeStyle="#0000FF";
			}
			else if(i==3){
				game_ctx.strokeStyle="#FFFF00";
			}
			else if(i==4){
				game_ctx.strokeStyle="#00FFFF";
			}
			else if(i==5){
				game_ctx.strokeStyle="#000000";
			}
			else if(i==6){
				game_ctx.strokeStyle="#004400";
			}
			else if(i==7){
				game_ctx.strokeStyle="#000044";
			}
			else if(i==8){
				game_ctx.strokeStyle="#444400";
			}
			else if(i==9){
				game_ctx.strokeStyle="#004444";
			}
			else if(i==10){
				game_ctx.strokeStyle="#444444";
			}

			game_ctx.arc(positions[i][j].x,positions[i][j].y,10,0,Math.PI*2);
			game_ctx.stroke();
			if(j==6){
				game_ctx.globalAlpha = 0.5;
				game_ctx.fillStyle = "red";
				game_ctx.fill();
				game_ctx.globalAlpha = 1.0;
			}
			if(j==4){
				game_ctx.globalAlpha = 0.5;
				game_ctx.fillStyle = "blue";
				game_ctx.fill();
				game_ctx.globalAlpha = 1.0;
			}
			game_ctx.strokeStyle="#000000";
		}
	}
}



PlotPoints();
//ColorItUp();

DrawBoardLines();
//LineTest();

function IsClickValid(mouse){
	for(var i=0;i<11;i++){
		for(var j=0;j<11;j++){
			if(positions[i][j].x==-1){
				continue;
			}
			if(positions[i][j].x-altitude/2<mouse.x&&positions[i][j].x+altitude/2>mouse.x
				&&positions[i][j].y-altitude/2<mouse.y&&positions[i][j].y+altitude/2>mouse.y){
					game_ctx.beginPath();
					game_ctx.arc(positions[i][j].x,positions[i][j].y,altitude/2,0,Math.PI*2);
					game_ctx.stroke();
			}
		}
	}
}

function getCanvasMousePosition (event) {
  var rect = game_canvas.getBoundingClientRect();

  return {
    x: event.clientX - rect.left,
    y: event.clientY - rect.top
  }
}

document.addEventListener('click', function(event) {
        lastDownTarget = event.target;
        if(lastDownTarget == game_canvas) {
        	var canvasMousePosition = getCanvasMousePosition(event);
        	IsClickValid(canvasMousePosition);
            /*game_ctx.beginPath();
			game_ctx.arc(canvasMousePosition.x,canvasMousePosition.y,10,0,Math.PI*2);
			game_ctx.stroke();*/
        }
    }, false);

