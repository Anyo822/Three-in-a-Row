#pragma once
#include <QAbstractListModel>
#include <deque>
#include <vector>
#include <QColor>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

class GameBoard : public QAbstractListModel
{
    Q_OBJECT

public:
    using Position = std::pair<size_t, size_t>;
    using IteratorPosition = std::pair<std::deque<std::deque<QColor>>::iterator, std::deque<QColor>::iterator>;

    static constexpr size_t defaultPuzzleDimension = 8;

    GameBoard(QObject *parent = nullptr, size_t board_dimension = defaultPuzzleDimension);

    int rowCount(const QModelIndex &parent = QModelIndex {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DecorationRole) const override;

    Q_INVOKABLE void shuffle();

    void generateBoard();
    bool generationCheck();
    Q_INVOKABLE bool matchCheck();
    bool matchFound();
    void getMarkedTiles();
    Q_INVOKABLE void removeMarkedTiles();
    Q_INVOKABLE void addNewTiles();
    Q_INVOKABLE void switchTiles(int indexFrom, int indexTo);
    Q_INVOKABLE bool makeMove(int indexFrom, int indexTo);

    Position getRowCol(const size_t index) const;
    int getIndex(const Position position) const;

signals:
    void wrongMove(int indexFrom, int indexTo);

private:
    void readJson();
    QColor getRandomColor();
    bool isAdjacent(const Position f, const Position s);
    int setCellColumns(int rowIndex, int columntIndex, int initialValueOfMathcedBlocks);
    int setCellRows(int rowIndex, int columntIndex, int initialValueOfMathcedBlocks);

    void generateMatches();

    size_t m_boardWidth;
    size_t m_boardHeight;
    std::mt19937 generator;

    QList<QList<QColor>> m_board;
    std::deque<QColor> m_colors;
    std::deque<Position> m_markedTiles;
    QList<QList<int>> m_matchedRows;
    QList<QList<int>> m_matchedColumns;
    QList<QList<int>> m_matchedTiles;
};
