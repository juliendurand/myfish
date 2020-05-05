import json

import perftdiff

if __name__ == '__main__':
	with open('test/movegen_tests.json', 'r') as json_testcases:
		tests = json.load(json_testcases)
		for test in tests:
			results = perftdiff.perft(perftdiff.Myfish, test['fen'], '', test['depth']).values()
			nodes = sum(map(int, results))
			success = nodes == test['nodes']
			print('OK' if success else 'FAILED', ':', test['fen'], test['depth'], nodes)
			assert success