#include "booksproxymodel.hpp"

/* explicit  */ BooksProxyModel::BooksProxyModel(QObject* parent /* = nullptr*/) {}

BooksProxyModel::~BooksProxyModel() /* override  */ = default;

/* [[nodiscard]] */ bool BooksProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const /* override  */
{
    // TODO доделать
    qDebug() << "filterAcceptsRow";

    // concatenation of all columns values
    QString comp_string;

    auto columns = sourceModel()->columnCount();
    for (int i = 0; i < columns; ++i) {
        comp_string += ' ';
        comp_string += sourceModel()->index(source_row, i, source_parent).data().toString();
    }

    auto regex = filterRegularExpression();

    qDebug() << "regex:" << regex;

    auto match = regex.match(comp_string);

    qDebug() << "is match:" << match.hasMatch();

    return true;
}
