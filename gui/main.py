import subprocess
import time

from flask import Flask, render_template, request, send_file


app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/play')
def play():
	print("play")
	moves = request.args.get('moves')
	print(moves)
	return search(moves)


def search(moves):
	engine = subprocess.Popen(
	    #'./../../Stockfish/src/stockfish',
	    '../bin/myfish',
	    universal_newlines=True,
	    stdin=subprocess.PIPE,
	    stdout=subprocess.PIPE,
	)
	put(engine, 'uci')
	put(engine, 'ucinewgame')
	put(engine, 'position startpos moves ' + moves)
	bestmove = put(engine, 'go movetime 100')
	while not bestmove:
		time.sleep(0.1)
		bestmove = put(engine, 'isready')
	engine.kill()
	return bestmove


def put(engine, command):
    #print('\nyou:\n\t'+command)
    engine.stdin.write(command+'\n')
    engine.stdin.write('isready\n')
    engine.stdin.flush()
    #print('\nengine:')
    while True:
        text = engine.stdout.readline().strip()
        if text == 'readyok':
        	break
        if text !='':
        	tokens = text.split(' ')
        	if tokens[0] == 'bestmove':
        		return tokens[1]
        	#print('\t'+text)
    return None


if __name__ == '__main__':
	print(search('e2e4'))