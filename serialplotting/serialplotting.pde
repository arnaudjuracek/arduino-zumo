import processing.serial.*;

Serial serial;
String portName = "COM29";
PGraphics p;
String s;

float
	x, y = 0,
	targetY = 0,
	py = 0,
	easing = .05;

String[] list = new String[1600];


void setup(){
	size(1600, 400);

	serial = new Serial(this, portName, 9600);
	serial.bufferUntil(';');

	background(255);
	textAlign(CENTER, CENTER);
	stroke(0);
	fill(0);

	p = createGraphics(width, height);
}


void draw(){
	background(255);
	image(plot(), 0, 0);

	stroke(255, 0, 0, 127);
	line(mouseX, 0, mouseX, height);
	if(list[mouseX] != null) text(list[mouseX], mouseX, mouseY + 50);
}

PGraphics plot(){
	if(x>width-1){
		p.background(255);
		x = 0;
	}
	x++;

	if(this.serial.available()>0) s = this.serial.readString();
	if(s!=null){
		if(int(s) > 0){
			targetY = int(s);
			list[int(x)] = s;
			py = y;
		}else{
			println(s);
		}
	}

	float dy = targetY - y;
	if(abs(dy)>1) y += dy * easing;

	p.beginDraw();
		p.stroke(0);
		p.line(x, py, x, y);
	p.endDraw();

	return p;
}