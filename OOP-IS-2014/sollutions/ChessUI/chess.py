from point import Point
from child_proc_communicate import NBSR
from subprocess import Popen, PIPE

class ChessBoard:

    def __init__(self, path):
        self.proc = Popen(path, stdin = PIPE, stdout = PIPE, stderr = PIPE, shell = False)
        self.nbsr = NBSR(self.proc.stdout)

        self._board = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                       ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
                       ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                       ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                       ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                       ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                       ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp'],
                       ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        self._finish_game = 0
        self.color_of_players = ('w', 'b')

    def __del__(self):
        self.command('exit', validate=True)

    def to_index(self, char):
        return ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'].index(char)

    def to_char(self, idx):
        return ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'][idx]

    def command(self, string, validate=False, multiple=False):
        self.proc.stdin.write(string + '\n')
        if multiple != False:
            out = []
            for idx in xrange(multiple-1):
                line = self.nbsr.readline(0.3)
                if not line:
                    raise RuntimeWarning(string)

                out.append(line)

            output = out
        else:
            output = self.nbsr.readline(0.3)
            print 'in:', string, ' |out:', output, ' -------------------'
            if not output:
                raise RuntimeWarning(string)

        if validate:
            return string.strip() == output.strip()

        return output


    def update_board(self):
        result = self.command('status')
        if 'playing' not in result:
            raise RuntimeWarning(result)

        print('drawing')
        res = self.command('display', multiple=65)
        for line in res[1:]:
            if line == '':
                continue
            if 'empty' in line:
                x, y, _ = line.split(' ')
                self._board[8-int(y)][ self.to_index(x)] = '  '
                #print 'set', x, y, 'empty'
            else:
                x, y, col, fig = line.split(' ')
                x = self.to_index(x)
                y = int(y) 
                self._board[8 - int(y)][ x] = col[0] + fig[0]
                #print 'set', x, y, col, fig
        result = self.command('status')

    def play(self, selected_figure, new_position):
        result = self.command('move ' +
            self.to_char(selected_figure.x) + ' ' + str(8 - selected_figure.y) + ' ' +
            self.to_char(new_position.x) + ' ' + str(8 - new_position.y), True
        )
        
        self.update_board()
        for line in self._board:
            print line

        return result != False
