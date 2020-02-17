#pragma once
#include <QAbstractListModel>
#include <deque>
#include <vector>
#include <random>
#include <QColor>


class GameBoard : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int boardWidth READ boardWidth CONSTANT)
    Q_PROPERTY(int boardHeight READ boardHeight CONSTANT)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(int moves READ moves NOTIFY movesChanged)

public:
    using Position = std::pair<int, int>;
    using IteratorPosition = std::pair<std::deque<std::deque<QColor>>::iterator, std::deque<QColor>::iterator>;\

    GameBoard(QObject * parent = nullptr);

    int rowCount(const QModelIndex & parent = QModelIndex {}) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DecorationRole) const override;

    Q_INVOKABLE void shuffle();
    Q_INVOKABLE void removeMarkedTiles();
    Q_INVOKABLE bool matchCheck();
    Q_INVOKABLE bool gameOverCheck();
    Q_INVOKABLE bool makeMove(int indexFrom, int indexTo);

    bool matchCheck(QList<QList<QColor>> & board);
    bool matchFound();
    void generateBoard();
    void getMarkedTiles();
    void addNewTiles();
    void switchTiles(int indexFrom, int indexTo);

    Position getRowCol(const size_t index) const;
    int getIndex(const Position position) const;

    size_t boardWidth() const;
    size_t boardHeight() const;
    size_t score() const;
    size_t moves() const;

signals:
    void wrongMove(int indexFrom, int indexTo);
    void scoreChanged();
    void movesChanged();

private:
    void readJson();
    QColor getRandomColor();
    bool isAdjacent(const Position f, const Position s);
    int setCellColumns(int rowIndex, int columntIndex, int initialValueOfMathcedBlocks, QList<QList<QColor>> & board);
    int setCellRows(int rowIndex, int columntIndex, int initialValueOfMathcedBlocks, QList<QList<QColor>> & board);

    void generateMatches(QList<QList<QColor>> & board);

    size_t m_boardWidth;
    size_t m_boardHeight;
    size_t m_score = 0;
    size_t m_moves = 0; 
    size_t const baseScorePerBall = 50;

    std::mt19937 generator;
    QList<QList<QColor>> m_board;
    QList<QList<QColor>> m_gameOverBoard;
    QList<QList<QColor>> m_moveBoard;
    QList<QColor> m_colors;
    QList   <Position> m_markedTiles;
    QList<QList<int>> m_matchedRows;
    QList<QList<int>> m_matchedColumns;
    QList<QList<int>> m_matchedTiles;
};
