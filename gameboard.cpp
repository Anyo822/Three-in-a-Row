#include "gameboard.h"
#include <random>
#include <iostream>
#include <QDebug>

GameBoard::GameBoard(QObject *parent, size_t board_dimension)
    : QAbstractListModel (parent),
      m_dimension {board_dimension},
      generator(std::chrono::system_clock::now().time_since_epoch().count()),
      randomColor(0, m_colors.size() - 1)
{
//    m_board.reserve(board_dimension);
//    //reserve may behave differently from resize
//    for (int i = 0; i < board_dimension; ++i) {
//        m_board[i].reserve(board_dimension);
//    }

    for (int i = 0; i < board_dimension; ++i) {
        m_board.append(QList<QColor>());

        for (int j = 0; j < board_dimension; ++j) {
            m_board[i].append(QColor());
        }
    }

    shuffle();
    //test
}

void GameBoard::generateBoard()
{
    beginResetModel();

    for (auto& yElement : m_board) {
        for (auto& xElement : yElement) {
            xElement = getRandomColor();
        }
    }

    endResetModel();
}

bool GameBoard::generationCHeck()
{
    for (int y = 0; y < m_board.size(); ++y) {
        for (int x = 0; x < m_board[0].size(); ++x) {
            size_t offset = 1;
            markedTiles.push_back(Position(y, x));

            while (offset + x < m_board[0].size() && m_board[y][x] == m_board[y][x + offset]) {
                offset++;

                markedTiles.push_back(Position(y, x + offset));

                if (offset >= 3 && (offset + x) >= m_board[0].size()) { //or m_board[y][x] != m_board[y][x + offset])
                    return true;
                }
                else {
                    markedTiles.clear();
                }
            }

            offset = 1;
            markedTiles.push_back(Position(y, x));

            while (offset + y < m_board.size() && m_board[y][x] == m_board[y + offset][x]) {
                offset++;

                markedTiles.push_back(Position(y + offset, x));
                if (offset >= 3 && (offset + y) >= m_board.size()) { //or m_board[y][x] != m_board[y][x + offset])
                    return true;
                }
                else {
                    markedTiles.clear();
                }
            }

            //add tile marking for deletion
        }
    }

    /*
      for (auto y = m_board.begin(); y != m_board.end(); ++y) {
        for (auto x = y->begin(); x != y->end(); ++x) {
            auto xOffset = x + 1;

            while (xOffset != m_board[0].end() && x->value() == xOffset->value()) {
                xOffset++;

                if (std::distance(x, xOffset) >= 3)
                    return true;
            }

            size_t xPosition = std::distance(y->begin(), x);
            auto yOffset = y + 1;
            auto yOffsettedX = yOffset->begin() + xPosition;

            while (yOffset != m_board.end() && x->value() == yOffsettedX->value()) {
                yOffset++;
                yOffsettedX = yOffset->begin() + xPosition;

                if (std::distance(y, yOffset) >= 3)
                    return true;
            }
        }
    }
    */





    return false;
}

void GameBoard::removeMarkedTiles()
{
    for (auto& tile : markedTiles) {
        m_board[tile.first].removeAt(tile.second);
    }
}

void GameBoard::addNewTiles()
{
    for (auto& tile : markedTiles) {
        m_board[tile.first].insert(tile.second, getRandomColor());
    }
}

void GameBoard::switchTiles(int indexFrom, int indexTo)
{
    Position positionFrom = getRowCol(indexFrom);
    Position positionTo = getRowCol(indexTo);

    int offsetDirection = indexTo - indexFrom;
    int offset = offsetDirection == -1 ? 1 : 0;

    beginMoveRows(QModelIndex {}, indexTo, indexTo, QModelIndex {}, indexFrom + offset);
    endMoveRows();

    if (std::abs(indexFrom - indexTo) > 1) {
        offset = offsetDirection > 0 ? 1 : 0;
        beginMoveRows(QModelIndex {}, indexFrom + offset, indexFrom + offset, QModelIndex {}, indexTo + offset);
        endMoveRows();
    }

    std::swap(m_board[positionFrom.first][positionFrom.second], m_board[positionTo.first][positionTo.second]);
}

GameBoard::Position GameBoard::getRowCol(size_t index) const
{
    Q_ASSERT(m_dimension > 0);
    size_t row = index / m_dimension;
    size_t column = index % m_dimension;

    return std::make_pair(row, column);
}

QColor GameBoard::getRandomColor()
{
    return m_colors[randomColor(generator)];
}

int GameBoard::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_board.size() * m_board[0].size();
}

QVariant GameBoard::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DecorationRole) {
        return {};
    }

    Position indexPosition = getRowCol(index.row());

    return QVariant::fromValue(m_board[indexPosition.first][indexPosition.second]);
}

void GameBoard::shuffle()
{
    do {
        generateBoard();
    } while (generationCHeck());
}