#include "gameboard.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

GameBoard::GameBoard(QObject * parent)
    : QAbstractListModel (parent)
    , generator(std::chrono::system_clock::now().time_since_epoch().count())
{
    readJson();

    for (size_t i = 0; i < m_boardHeight; ++i) {
        m_board.append(QList<QColor>());
        m_matchedRows.append(QList<int>());
        m_matchedColumns.append(QList<int>());
        m_matchedTiles.append(QList<int>());

        for (size_t j = 0; j < m_boardWidth; ++j) {
            m_board[i].append(QColor());
            m_matchedRows[i].append(1);
            m_matchedColumns[i].append(1);
            m_matchedTiles[i].append(1);
        }
    }

    shuffle();
}

void GameBoard::generateBoard()
{
    beginResetModel();

    for (auto & yElement : m_board) {
        for (auto & xElement : yElement) {
            xElement = getRandomColor();
        }
    }

    endResetModel();
}

bool GameBoard::matchCheck()
{
    generateMatches(m_board);

    if (matchFound()) {
        getMarkedTiles();
        return true;
    }
    else {
        return false;
    }
}

bool GameBoard::matchCheck(QList<QList<QColor> > & board)
{
    generateMatches(board);

    return matchFound();
}

bool GameBoard::gameOverCheck()
{
    m_gameOverBoard = m_board;
    for (size_t y = 0; y < m_boardHeight; ++y) {
        for (size_t x = 0; x < m_boardWidth; ++x) {
            if (x < m_boardWidth - 1) {
                std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y][x + 1]);
                if (matchCheck(m_gameOverBoard)) {
                    return false;
                }
                else {
                    std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y][x + 1]);
                }
            }
            if (x > 0) {
                std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y][x - 1]);
                if (matchCheck(m_gameOverBoard)) {
                    return false;
                }
                else {
                    std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y][x - 1]);
                }
            }

            if (y < m_boardHeight - 1) {
                std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y + 1][x]);
                if (matchCheck(m_gameOverBoard)) {
                    return false;
                }
                else {
                    std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y + 1][x]);
                }
            }
            if (y > 0) {
                std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y - 1][x]);
                if (matchCheck(m_gameOverBoard)) {
                    return false;
                }
                else {
                    std::swap(m_gameOverBoard[y][x], m_gameOverBoard[y - 1][x]);
                }
            }
        }
    }
    return true;
}

bool GameBoard::matchFound()
{
    for (auto & list : m_matchedTiles) {
        if (std::any_of(list.begin(), list.end(), [](int value) { return value >= 3; })) {
            return true;
        }
    }

    return false;
}

void GameBoard::getMarkedTiles()
{
    m_markedTiles.clear();

    for (int y = 0; y < m_matchedTiles.size(); ++y) {
        for (int x = 0; x < m_matchedTiles[y].size(); ++x) {
            if (m_matchedTiles[y][x] >= 3) {
                m_markedTiles.push_front(Position(y, x));
            }
        }
    }
}

void GameBoard::removeMarkedTiles()
{
    m_score += m_markedTiles.size() * baseScorePerBall;;
    emit scoreChanged();

    for (auto & tile : m_markedTiles) {
        beginRemoveRows(QModelIndex(), getIndex(tile), getIndex(tile));
        m_board[tile.first].removeAt(tile.second);
        endRemoveRows();
    }

    addNewTiles();
}

void GameBoard::addNewTiles()
{    
    for (auto tile = m_markedTiles.rbegin() ; tile != m_markedTiles.rend(); ++tile) {
        beginInsertRows(QModelIndex(), getIndex(Position(tile->first, 0)), getIndex(Position(tile->first, 0)));
        m_board[tile->first].push_front(getRandomColor());
        endInsertRows();
    }
}
//search for sequences where tile->first == each other until not OR tile->second
//then erase such sequences

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
    Position positionFrom = getRowCol(indexFrom);
    Position positionTo = getRowCol(indexTo);

    if (!isAdjacent(positionFrom, positionTo)) {
        return false;
    }

    m_moveBoard = m_board;
    std::swap(m_moveBoard[positionFrom.first][positionFrom.second], m_moveBoard[positionTo.first][positionTo.second]);

    generateMatches(m_moveBoard);

    if (matchFound()) {
        m_moves += 1;
        emit movesChanged();

        getMarkedTiles();
        switchTiles(indexFrom, indexTo);

        return true;
    }
    else {
        emit wrongMove(indexFrom, indexTo);

        return false;
    }
}

GameBoard::Position GameBoard::getRowCol(size_t index) const
{
    Q_ASSERT(m_boardHeight > 0 &&  m_boardWidth > 0);
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
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, & errorPtr);
    if (doc.isNull()) {
        qDebug() << "Parse failed";
    }
    QJsonObject rootObj = doc.object();

    m_boardWidth = rootObj.value("width").toInt();
    m_boardHeight = rootObj.value("height").toInt();

    QJsonArray ptsArray = rootObj.value("colors").toArray();

    for (const auto & element : ptsArray) {
        m_colors.emplace_back(element.toString());
    }
}

QColor GameBoard::getRandomColor()
{
    return m_colors[std::uniform_int_distribution<int>(0, m_colors.size() - 1)(generator)];
}

int GameBoard::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return m_board.size() * m_board[0].size();
}

QVariant GameBoard::data(const QModelIndex & index, int role) const
{
    if (!index.isValid() || role != Qt::DecorationRole) {
        return {};
    }

    Position indexPosition = getRowCol(index.row());

    if (indexPosition.first < m_board.size() &&  indexPosition.second < m_board[indexPosition.first].size()) {
        return QVariant::fromValue(m_board[indexPosition.first][indexPosition.second]);
    }

    return {};
}

void GameBoard::shuffle()
{
    m_score = 0;
    m_moves = 0;
    emit scoreChanged();
    emit movesChanged();

    do {
        generateBoard();
        generateMatches(m_board);
    } while (matchFound());

//    if (gameOverCheck()) {
//        shuffle();
//    }
//    generateMatches(m_board);
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

int GameBoard::setCellRows(int rowIndex, int columntIndex, int initialValueOfMathcedBlocks, QList<QList<QColor>> &  board)
{
    if (columntIndex >= m_board[rowIndex].size() - 1) {
        m_matchedRows[rowIndex][columntIndex] = initialValueOfMathcedBlocks;

        return m_matchedRows[rowIndex][columntIndex];
    }

    bool isNextCellTheSame = board[rowIndex][columntIndex] == board[rowIndex][columntIndex + 1];

    if (isNextCellTheSame) {
        m_matchedRows[rowIndex][columntIndex] = setCellRows(rowIndex, columntIndex + 1, initialValueOfMathcedBlocks + 1, board);
    }
    else {
        setCellRows(rowIndex, columntIndex + 1, 1, board);
        m_matchedRows[rowIndex][columntIndex] = initialValueOfMathcedBlocks;
    }

    return m_matchedRows[rowIndex][columntIndex];
}

int GameBoard::setCellColumns(int rowIndex, int columntIndex, int initialValueOfMathcedBlocks, QList<QList<QColor>> &  board)
{
    if (rowIndex >= m_board.size() - 1) {
        m_matchedColumns[rowIndex][columntIndex] = initialValueOfMathcedBlocks;

        return m_matchedColumns[rowIndex][columntIndex];
    }

    bool isNextCellTheSame = board[rowIndex][columntIndex] == board[rowIndex + 1][columntIndex];

    if (isNextCellTheSame) {
        m_matchedColumns[rowIndex][columntIndex] = setCellColumns(rowIndex + 1, columntIndex, initialValueOfMathcedBlocks + 1, board);
    }
    else {
        setCellColumns(rowIndex + 1, columntIndex, 1, board);
        m_matchedColumns[rowIndex][columntIndex] = initialValueOfMathcedBlocks;
    }

    return m_matchedColumns[rowIndex][columntIndex];
}

void GameBoard::generateMatches(QList<QList<QColor>> &  board)
{
    for (size_t i = 0; i < m_boardHeight; ++i) {
        setCellRows(i, 0, 1, board);
    }

//    for (const auto & el : m_matchedRows) {
//        qDebug() << el;
//    }

//    qDebug() << "-----------ROWS----------";

    for (size_t i = 0; i < m_boardWidth; ++i) {
        setCellColumns(0, i, 1, board);
    }

//    for (const auto & el : m_matchedColumns) {
//        qDebug() << el;
//    }

//    qDebug() << "-----------COLUMNS----------";

    for (size_t y = 0; y < m_boardHeight; ++y) {
        for (size_t x = 0; x < m_boardWidth; ++x) {
            m_matchedTiles[y][x] = std::max(m_matchedRows[y][x], m_matchedColumns[y][x]);
        }
    }

    for (const auto & el : m_matchedTiles) {
        qDebug() << el;
    }

    qDebug() << "----------FULL-----------";
}

size_t GameBoard::moves() const
{
    return m_moves;
}

size_t GameBoard::score() const
{
    return m_score;
}

size_t GameBoard::boardHeight() const
{
    return m_boardHeight;
}

size_t GameBoard::boardWidth() const
{
    return m_boardWidth;
}
