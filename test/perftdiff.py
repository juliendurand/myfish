import subprocess

#startpos = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
#startpos = '8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1'
#startpos = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"
startpos = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

Myfish = ('MyFish   ', './bin/myfish', 'position fen {} moves {}\nperft {}\n')
Stockfish = ('StockFish', './../Stockfish/src/stockfish',
             'position fen {} moves {}\ngo perft {}\n')


def perft(engine, position, moves, depth):
    process = subprocess.run(engine[1],
                             input=engine[2].format(position, moves, depth),
                             stdout=subprocess.PIPE, encoding='utf8')
    output = process.stdout
    results = {}
    for line in output.split('\n')[1:]:
        if not line:
            break
        try:
            move, nodes = line.split(': ')
        except Exception as e:
            print(line)
            raise e
        if results.get(move) is not None:
            print("double move : " + move)
            raise Exception("Double moves !?")
        results[move] = nodes
    # print(engine[0], moves, results)
    return results


def diff(engine1, r1, engine2, r2, moves):
    all_moves = set(r1.keys()) | set(r2.keys())
    # print(all_moves)
    for m in sorted(all_moves):
        if not r1.get(m):
            print('Move {} missing in {}'.format(moves + [m], engine1[0]))
            exit(1)
        if not r2.get(m):
            print('Move {} missing in {}'.format(moves + [m], engine2[0]))
            exit(1)
        if r1[m] != r2[m]:
            ds = 'Different number of nodes for moves {} -> {}: {}, {}: {}'
            print(ds.format(m, engine1[0], r1[m], engine2[0], r2[m]))
            return m


def compare(depth):
    moves = []
    error = False
    while depth > 0:
        r1 = perft(Myfish, startpos, ' '.join(moves), depth)
        r2 = perft(Stockfish, startpos, ' '.join(moves), depth)
        m = diff(Myfish, r1, Stockfish, r2, moves)
        if not m:
            if error:
                print(moves)
                print(Myfish[0], r1)
                print(Stockfish[0], r2)
                return False
            else:
                print("OK - Same results")
                return True
        else:
            error = True
            depth -= 1
            moves += [m]
    return False


for i in range(1, 7):
    print('perft ', i, ': ')
    if not compare(i):
        break
