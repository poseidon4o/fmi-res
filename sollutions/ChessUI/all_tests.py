import unittest

from chess import ChessBoard
from point import Point

from valid_move import*

#                              INDEXES OF THE BOARD
#
#                     0      1    2      3    4     5      6     7
#               0  [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
#               1   ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
#               2   ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
#               3   ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
#               4   ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
#               5   ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
#               6   ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp'],
#               7   ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]


class TestBoard(unittest.TestCase):

    def test_get_figure_from_board(self):

        board = ChessBoard()

        blackPawn = board.get_figure_from_board(Point(1, 1))
        whiteRock = board.get_figure_from_board(Point(7, 7))
        blackBishop = board.get_figure_from_board(Point(5, 0))
        whiteQueen = board.get_figure_from_board(Point(3, 7))
        whiteSpace = board.get_figure_from_board(Point(4, 4))

        self.assertEqual(blackPawn, "bp")
        self.assertEqual(whiteRock, "wr")
        self.assertEqual(blackBishop, "bb")
        self.assertEqual(whiteQueen, "wq")
        self.assertEqual(whiteSpace, "  ")

        self.assertNotEqual(blackPawn, "wp")
        self.assertNotEqual(blackBishop, "wb")
        self.assertNotEqual(whiteQueen, "  ")

    def test_set_end_of_the_game(self):
        board = ChessBoard()

        # at the beginning of the game the state is zero
        self.assertNotEqual(1, board._finish_game)

        # get one of the Kings
        board.set_end_of_the_game("wk")
        self.assertEqual(1, board._finish_game)

    def test_is_end_of_the_game(self):

        board = ChessBoard()

        self.assertFalse(board.is_end())
        # get one of the Kings
        board.set_end_of_the_game("wk")
        self.assertTrue(board.is_end())

    def test_get_the_board(self):
        board = ChessBoard()
        theBoard = board.get_board()

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp'],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        self.assertEqual(testBoard, theBoard)

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  ', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', 'bp', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp'],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        self.assertNotEqual(testBoard, theBoard)

    def test_get_color_of_player(self):

        board = ChessBoard()

        self.assertEqual(board.get_color_of_player(0), "w")
        self.assertEqual(board.get_color_of_player(1), "b")
        self.assertNotEqual(board.get_color_of_player(0), "b")

    def test_get_color_of_figure_of_certain_cell(self):
        board = ChessBoard()

        self.assertEqual(board.get_current_figure_color(7, 7), "w")
        self.assertEqual(board.get_current_figure_color(1, 1), "b")
        self.assertNotEqual(board.get_current_figure_color(3, 3), "b")

    def test_allowed_selection_cell(self):

        # Test 1
        board = ChessBoard()
        selected_cell = Point(0, 0)
        player = 0
        self.assertFalse(board.allowed_selection(selected_cell, player))

        # Test 2
        selected_cell = Point(7, 7)
        player = 0
        self.assertTrue(board.allowed_selection(selected_cell, player))

        # Test 3
        selected_cell = Point(4, 4)
        player = 1
        self.assertFalse(board.allowed_selection(selected_cell, player))

    def test_get_allowed_color_of_player(self):

        board = ChessBoard()
        player = 0
        board.set_allowed_color(player)

        # Test 1 & 2
        self.assertEqual(board.get_allawod_color(), "w")
        self.assertNotEqual(board.get_allawod_color(), "b")

        # Test 3
        player = 1
        board.set_allowed_color(player)
        self.assertNotEqual(board.get_allawod_color(), "w")

    def test_set_allowed_color_of_player(self):

        board = ChessBoard()
        player = 0
        board.set_allowed_color(player)

        # Test 1 & 2
        self.assertEqual(board.get_allawod_color(), "w")
        self.assertNotEqual(board.get_allawod_color(), "b")

        # Test 3
        player = 1
        board.set_allowed_color(player)
        self.assertEqual(board.get_allawod_color(), "b")

    def test_invalid_move_black_pawn(self):
        board = ChessBoard()
        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', 'wp', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', '  ', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp'],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        # Test 1
        selected_figure = Point(1, 1)
        new_position = Point(3, 1)
        motion = board._make_motion(selected_figure, new_position)
        self.assertTrue(board.invalid_move_black_pawn(motion, new_position))

        # Test 2
        new_position = Point(3, 2)
        motion = board._make_motion(selected_figure, new_position)
        self.assertTrue(board.invalid_move_black_pawn(motion, new_position))

        # Test 3
        selected_figure = Point(2, 2)
        new_position = Point(2, 3)
        motion = board._make_motion(selected_figure, new_position)
        self.assertFalse(board.invalid_move_black_pawn(motion, new_position))

    def test_make_a_promotion_of_queen(self):
        board = ChessBoard()
        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'wp', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  ', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', '  ', 'wp'],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        # Test 1
        selected_figure = Point(6, 0)
        self.assertEqual(board.get_figure_from_board(selected_figure), "wp")

        # Test 2
        board._promotion_queen(selected_figure, "wq")
        self.assertEqual(board.get_figure_from_board(selected_figure), "wq")

    def test_can__get_with_white_pawn(self):

        board = ChessBoard()
        # test 1
        white_pawn_position = Point(2, 2)
        black_pawn_position = Point(3, 1)
        motion = board._make_motion(white_pawn_position, black_pawn_position)

        self.assertTrue(board._get_with_white_pawn(motion, black_pawn_position))

        # test 2
        white_pawn_position = Point(3, 3)
        black_pawn_position = Point(4, 2)
        motion = board._make_motion(white_pawn_position, black_pawn_position)

        self.assertFalse(board._get_with_white_pawn(motion, black_pawn_position))

        # test 3
        white_pawn_position = Point(2, 2)
        black_pawn_position = Point(2, 1)
        motion = board._make_motion(white_pawn_position, black_pawn_position)

        self.assertFalse(board._get_with_white_pawn(motion, black_pawn_position))

    def test_can__get_with_black_pawn(self):

        board = ChessBoard()
        # test 1
        black_pawn_position = Point(5, 5)
        white_pawn_position = Point(4, 6)
        motion = board._make_motion(black_pawn_position, white_pawn_position)

        self.assertTrue(board._get_with_black_pawn(motion, white_pawn_position))

        # test 2
        black_pawn_position = Point(5, 5)
        white_pawn_position = Point(5, 6)
        motion = board._make_motion(black_pawn_position, white_pawn_position)

        self.assertFalse(board._get_with_black_pawn(motion, white_pawn_position))

        # test 3
        black_pawn_position = Point(3, 3)
        white_pawn_position = Point(4, 4)
        motion = board._make_motion(black_pawn_position, white_pawn_position)

        self.assertFalse(board._get_with_black_pawn(motion, white_pawn_position))

    def test_clean_cell_after_get_or_move_figure(self):

        board = ChessBoard()

        cell_to_clean = Point(1, 1)
        board.clean_cell(cell_to_clean)
        self.assertEqual(board.get_figure_from_board(cell_to_clean), "  ")

    def test_get_color_of_figure_on_the_certain_cell(self):

        board = ChessBoard()

        cell = Point(3, 6)  # by default is "wp"

        self.assertEqual(board.get_color_of_figure(cell), "w")

    def test__finally_move(self):
        board = ChessBoard()
        selected_figure = Point(3, 6)
        new_position = Point(3, 4)

        board._finally_move(new_position, selected_figure)
        self.assertEqual(board.get_figure_from_board(new_position), "wp")

    def test_whether_knight_try_to_moving_on_own_figure(self):
        board = ChessBoard()

        knight = Point(6, 7, "wn")
        new_position = Point(4, 6)
        self.assertTrue(board.knight_invalid_move(new_position, knight))

    def test_if_try_to_jump_over_enemy_figure(self):

        board = ChessBoard()
        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'wr'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', '  ']]

        set_board(board, testBoard)

        selected_figure = Point(7, 4, "wr")
        final_position = Point(7, 0)

        next_cell_while_moving = Point(7, 3)

        motion = board._make_motion(selected_figure, final_position)

        step_x, step_y = board._set_step_to_move(motion)
        prepare_moving = Point(step_x, step_y)

        self.assertTrue(board._try_jump_enemy(
            next_cell_while_moving, selected_figure, prepare_moving, motion))

    def test_if_try_to_jump_over_your_figure(self):

        board = ChessBoard()
        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'wp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'wr'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', '  ']]

        set_board(board, testBoard)

        selected_figure = Point(7, 4, "wr")

        next_cell_while_moving = Point(7, 3)

        self.assertTrue(board._try_jump_yours(
            next_cell_while_moving, selected_figure))

    def test_waiting_to_make_a_full_turn(self):

        board = ChessBoard()
        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        selected_figure = Point(7, 7, "wr")
        final_position = Point(7, 3)
        motion = board._make_motion(selected_figure, final_position)

        step_x, step_y = board._set_step_to_move(motion)
        prepare_moving = Point(step_x, step_y)

        self.assertTrue(
            board.waiting_to_finish_this_turn(prepare_moving, motion))

    def test_whether_the_move_would_be_make_by_rules(self):

        board = ChessBoard()
        white_pawn = Point(6, 6, "wp")
        target_cell = Point(6, 5)
        motion = board._make_motion(white_pawn, target_cell)
        # Test 1
        self.assertTrue(
            board._checking_the_move_is_correct(white_pawn, motion))

        # Test 2
        white_pawn = Point(6, 7, "wn")
        target_cell = Point(5, 5)
        motion = board._make_motion(white_pawn, target_cell)

        self.assertTrue(
            board._checking_the_move_is_correct(white_pawn, motion))

        # Test 3
        white_knight = Point(6, 7, "wn")
        target_cell = Point(6, 5)
        motion = board._make_motion(white_knight, target_cell)

        self.assertFalse(
            board._checking_the_move_is_correct(white_knight, motion))

        # Test 4
        black_pawn = Point(1, 1, "bp")
        target_cell = Point(1, 5)
        motion = board._make_motion(black_pawn, target_cell)

        self.assertFalse(
            board._checking_the_move_is_correct(black_pawn, motion))

        # Test 5
        white_bishop = Point(3, 5, "wb")
        target_cell = Point(4, 4)
        motion = board._make_motion(white_bishop, target_cell)

        self.assertTrue(
            board._checking_the_move_is_correct(white_bishop, motion))

        # Test 6
        white_bishop = Point(3, 5, "wb")
        target_cell = Point(4, 5)
        motion = board._make_motion(white_bishop, target_cell)

        self.assertFalse(
            board._checking_the_move_is_correct(white_bishop, motion))

        # Test 7
        white_rock = Point(3, 5, "wr")
        target_cell = Point(4, 4)
        motion = board._make_motion(white_rock, target_cell)

        self.assertFalse(
            board._checking_the_move_is_correct(white_rock, motion))

        # Test 8
        white_queen = Point(3, 5, "wq")
        target_cell = Point(4, 4)
        motion = board._make_motion(white_queen, target_cell)

        self.assertTrue(
            board._checking_the_move_is_correct(white_queen, motion))

        # Test 9
        white_king = Point(3, 5, "wk")
        target_cell = Point(3, 3)
        motion = board._make_motion(white_king, target_cell)

        self.assertFalse(
            board._checking_the_move_is_correct(white_king, motion))

        # Test 10
        white_king = Point(3, 5, "wk")
        target_cell = Point(4, 4)
        motion = board._make_motion(white_king, target_cell)

        self.assertTrue(
            board._checking_the_move_is_correct(white_king, motion))

    def test_invalid_move_white_pawn(self):
        board = ChessBoard()

        # Test 1
        white_pawn = Point(1, 3)
        new_position = Point(1, 1)
        motion = board._make_motion(white_pawn, new_position)

        self.assertTrue(board.invalid_move_white_pawn(motion, new_position))

        # Test 2
        new_position = Point(1, 0)
        motion = board._make_motion(white_pawn, new_position)
        self.assertTrue(board.invalid_move_white_pawn(motion, new_position))

        # Test 3
        new_position = Point(1, 4)
        motion = board._make_motion(white_pawn, new_position)

        self.assertTrue(board.invalid_move_white_pawn(motion, new_position))

        # Test 4
        new_position = Point(1, 2)
        motion = board._make_motion(white_pawn, new_position)

        self.assertFalse(board.invalid_move_white_pawn(motion, new_position))

    def test_valide_selected_cell(self):
        board = ChessBoard()
        player = 0
        board.set_allowed_color(player)
        selected_figure = Point(5, 5)

        # Test 1
        selected_figure.set_info("  ")
        self.assertTrue(board._invalid_selection(selected_figure))

        # Test 2
        selected_figure.set_info("wp")
        self.assertFalse(board._invalid_selection(selected_figure))

        # Test 3
        player = 1
        board.set_allowed_color(player)

        selected_figure.set_info("wp")
        self.assertTrue(board._invalid_selection(selected_figure))

    def test_calculate_of_motion_of_figure(self):

        board = ChessBoard()

        # Test 1
        start_position = Point(1, 1)
        destination = Point(1, 2)

        motion = board._make_motion(start_position, destination)

        self.assertEqual(motion.get_x(), 0)
        self.assertEqual(motion.get_y(), 1)

        # Test 2
        start_position = Point(1, 1)
        destination = Point(1, 5)

        motion = board._make_motion(start_position, destination)

        self.assertNotEqual(motion.get_x(), 1)
        self.assertEqual(motion.get_y(), 4)

    def test_play(self):
        board = ChessBoard()
        player = 0
        board.set_allowed_color(player)

        # Test 1
        white_figure = Point(2, 6, "wp")
        new_position = Point(2, 4)

        self.assertTrue(board.play(white_figure, new_position))

        # Test 2
        selected_figure = Point(-1, -1, "  ")
        new_position = Point(2, 4)

        self.assertFalse(board.play(selected_figure, new_position))

        # Test 3

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', '  ', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', 'bp', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)
        board.set_allowed_color(1)
        black_pawn = Point(3, 5, "bp")
        new_position = Point(4, 6)

        self.assertTrue(board.play(black_pawn, new_position))

        # Test 4

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', '  ', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', 'bp', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', 'wp', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)
        black_pawn = Point(3, 5, "bp")
        new_position = Point(4, 5)

        self.assertFalse(board.play(black_pawn, new_position))

        # Test 5

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)
        board.set_allowed_color(0)
        white_figure = Point(3, 2, "wp")
        new_position = Point(4, 1)

        self.assertTrue(board.play(white_figure, new_position))

        # Test 6

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp'],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_figure = Point(3, 2, "wp")
        new_position = Point(4, 2)

        self.assertFalse(board.play(white_figure, new_position))

        # Test 7

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  '],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'bp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_rock = Point(7, 7, "wr")
        new_position = Point(7, 1)

        self.assertFalse(board.play(white_rock, new_position))

        # Test 8

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  '],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'wp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_rock = Point(7, 7, "wr")
        new_position = Point(7, 1)

        self.assertFalse(board.play(white_rock, new_position))

        # Test 9

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', 'br'],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', '  '],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', 'wp'],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_knight = Point(6, 7, "wn")
        new_position = Point(4, 6)

        self.assertFalse(board.play(white_knight, new_position))

        # Test 10

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', '  '],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'wp'],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_pawn = Point(7, 1, "wp")
        new_position = Point(7, 0)

        self.assertTrue(board.play(white_pawn, new_position))

        # Test 11

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', '  '],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'wp'],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_pawn = Point(7, 1, "wp")
        new_position = Point(6, 0)

        self.assertTrue(board.play(white_pawn, new_position))

        # Test 12

        testBoard = [['br', 'bn', 'bb', 'bq', 'bk', 'bb', 'bn', '  '],
                     ['bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'bp', 'wp'],
                     ['  ', '  ', '  ', 'wp', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  '],
                     ['wp', 'wp', 'wp', '  ', 'wp', 'wp', 'wp', '  '],
                     ['wr', 'wn', 'wb', 'wq', 'wk', 'wb', 'wn', 'wr']]

        set_board(board, testBoard)

        white_pawn = Point(3, 3, "  ")
        new_position = Point(6, 0)

        self.assertFalse(board.play(white_pawn, new_position))


def set_board(to_set, board):
    to_set._board = board


class TestPoint(unittest.TestCase):

    def test_get_color_of_point(self):
        cell = Point(7, 7, "wr")

        self.assertEqual(cell.get_color(), "w")

    def test_get_type(self):
        cell = Point(7, 7, "wr")

        self.assertEqual(cell.get_type(), "r")

    def test_get_all_info_about_cell(self):

        cell = Point(7, 7, "  ")
        self.assertEqual(cell.get_figure(), "  ")

    def test_get_x(self):
        cell = Point(0, 0, "br")

        self.assertEqual(cell.get_x(), 0)

    def test_get_y(self):
        cell = Point(0, 0, "br")

        self.assertEqual(cell.get_y(), 0)

    def test_set_info_about_certain_cell(self):
        cell = Point(0, 0, "br")
        cell.set_info("wr")

        self.assertEqual(cell._info, "wr")

    def test_set_x(self):
        cell = Point(0, 0, "br")
        cell.set_x(1)

        self.assertNotEqual(cell.get_x(), 0)

    def test_set_y(self):
        cell = Point(0, 0, "br")
        cell.set_y(1)

        self.assertNotEqual(cell.get_y(), 0)

    def test_set_point(self):
        cell = Point(0, 0, "br")
        cell.set_point(1, 1)
        self.assertNotEqual(cell.get_y(), 0)

    def test_increase_x_and_y(self):
        cell = Point(0, 0, "br")
        cell.increase(1, 1)
        self.assertNotEqual(cell.get_y(), 0)

if __name__ == "__main__":
    unittest.main()
