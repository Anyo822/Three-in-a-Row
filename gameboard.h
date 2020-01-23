#pragma once
#include <QAbstractListModel>
#include <deque>
#include <vector>
#include <QColor>

class GameBoard : public QAbstractListModel
{
    Q_OBJECT

public:
    using Position = std::pair<size_t, size_t>;
    using IteratorPosition = std::pair<std::deque<std::deque<QColor>>::iterator, std::deque<QColor>::iterator>;

    static constexpr size_t defaultPuzzleDimension = 8;

    GameBoard(QObject *parent = nullptr, size_t board_dimension = defaultPuzzleDimension);

    int rowCount(const QModelIndex& parent = QModelIndex {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DecorationRole) const override;

    Q_INVOKABLE void shuffle();

    void generateBoard();
    bool generationCHeck();
    void removeMarkedTiles(std::deque<Position> &markedTiles);
    void addNewTiles(std::deque<IteratorPosition> &markedTiles);

    Position getRowCol(const size_t index) const;

private:
    const size_t m_dimension;
    std::mt19937 generator;
    const std::vector<QColor> m_colors {QColor("red"), QColor("green"),QColor("blue"),
                                  QColor("violet"), QColor("yellow")};
    std::uniform_int_distribution<int> randomColor;
    QList<QList<QColor>> m_board;
};
