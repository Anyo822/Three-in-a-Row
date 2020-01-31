#include "gameboard.h"
#include <random>
#include <iostream>
#include <QDebug>

GameBoard::GameBoard(QObject *parent, size_t board_dimension)
    : QAbstractListModel (parent)
    , generator(std::chrono::system_clock::now().time_since_epoch().count())
{
    readJson();

    for (int i = 0; i < m_boardHeight; ++i) {
        m_board.append(QList<QColor>());

        for (int j = 0; j < m_boardWidth; ++j) {
            m_board[i].append(QColor());
        }
    }

    shuffle();
}

void GameBoard::generateBoard()
{
    beginResetModel();

    for (auto &yElement : m_board) {
        for (auto &xElement : yElement) {
            xElement = getRandomColor();
        }
    }

    endResetModel();
}

bool GameBoard::generationCheck()
{
    for (int y = 0; y < m_board.size(); ++y) {
        for (int x = 0; x < m_board[y].size(); ++x) {
            size_t offset = 1;
            m_markedTiles.push_back(Position(y, x));

            while (//y < m_board.size() && x < m_board[y].size()
                   offset + x < m_board[y].size() && m_board[y][x] == m_board[y][x + offset]) {
                m_markedTiles.push_back(Position(y, x + offset));
                offset++;
            }
            if (offset >= 3) { //&& ((offset + x) >= m_board[y].size() || m_board[y][x] != m_board[y][x + offset])
                return true;
            }
            m_markedTiles.clear();

            offset = 1;
            m_markedTiles.push_back(Position(y, x));

            while (//y < m_board.size() && x < m_board[y].size()
                   offset + y < m_board.size() && x < m_board[y + offset].size()
                   && m_board[y][x] == m_board[y + offset][x]) {
                m_markedTiles.push_back(Position(y + offset, x));
                offset++;
            }
            if (offset >= 3) { //&& ((offset + y) >= m_board.size() || x >= m_board[offset + y].size() || m_board[y][x] != m_board[y + offset][x])
                return true;
            }
            m_markedTiles.clear();
        }
    }
    return false;
}

void GameBoard::removeMarkedTiles()
{
    for (auto &tile : m_markedTiles) {
        beginRemoveRows(QModelIndex(), getIndex(tile), getIndex(tile));
        m_board[tile.first].removeAt(tile.second);
        endRemoveRows();
    }
}

void GameBoard::addNewTiles()
{
    for (auto &tile : m_markedTiles) {
        beginInsertRows(QModelIndex(), getIndex(Position(tile.first, 0)), getIndex(Position(tile.first, 0)));
        m_board[tile.first].push_front(getRandomColor());
        endInsertRows();
    }
}

void GameBoard::switchTiles(int indexFrom, int indexTo)
{
    Position positionFrom = getRowCol(indexFrom);
    Position positionTo = getRowCol(indexTo);

    if (!isAdjacent(positionFrom, positionTo)) {
        return;
    }

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

bool GameBoard::makeMove(int indexFrom, int indexTo)
{
    bool switchFound = false;

    switchTiles(indexFrom, indexTo);

    while (generationCheck()) {
        removeMarkedTiles();
        addNewTiles();
        m_markedTiles.clear();
        switchFound = true;
    }

    if (!switchFound) {
        switchTiles(indexTo, indexFrom);

        return false;
    }

    return true;
}

GameBoard::Position GameBoard::getRowCol(size_t index) const
{
    Q_ASSERT(m_boardHeight > 0 && m_boardWidth > 0);
    size_t row = index / m_boardHeight;
    size_t column = index % m_boardWidth;

    return std::make_pair(row, column);
}

int GameBoard::getIndex(const GameBoard::Position position) const
{
    Q_ASSERT(m_boardHeight > 0);

    return position.first * m_boardHeight + position.second;
}

void GameBoard::readJson()
{
    QFile inFile(":/settings.json");
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qDebug() << "Parse failed";
    }
    QJsonObject rootObj = doc.object();

    m_boardHeight = rootObj.value("height").toInt();
    m_boardWidth = rootObj.value("width").toInt();

    QJsonArray ptsArray = rootObj.value("colors").toArray();

    for (const auto &element : ptsArray) {
        m_colors.emplace_back(element.toString());
    }
}

QColor GameBoard::getRandomColor()
{
    return m_colors[std::uniform_int_distribution<int>(0, m_colors.size() - 1)(generator)];
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

    if (indexPosition.first < m_board.size() && indexPosition.second < m_board[indexPosition.first].size()) {
        return QVariant::fromValue(m_board[indexPosition.first][indexPosition.second]);
    }

    return {};
}

void GameBoard::shuffle()
{
    do {
        generateBoard();
    } while (generationCheck());
}

bool GameBoard::isAdjacent(const Position f, const Position s)
{
    if (f == s) {
        return false;
    }

    const auto calcDistance = [](const size_t pos1, const size_t pos2) {
        return std::abs(static_cast<int>(pos1) - static_cast<int>(pos2));
    };

    return calcDistance(f.first, s.first) + calcDistance(f.second, s.second) == 1;
}
