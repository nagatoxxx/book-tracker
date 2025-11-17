#include "booksproxymodel.hpp"

/* explicit  */ BooksProxyModel::BooksProxyModel(QObject* parent /* = nullptr*/) {}

BooksProxyModel::~BooksProxyModel() /* override  */ = default;

/* [[nodiscard]] */ bool BooksProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_idx) const /* override  */
{
    qDebug() << "filterAcceptsRow";

    // concatenation of all columns values
    QString comp_string;

    const auto columns = sourceModel()->columnCount();
    for (int i = 0; i < columns; ++i) {
        comp_string += ' ';
        comp_string += sourceModel()->index(source_row, i, source_idx).data().toString();
    }

    const auto regex = filterRegularExpression();

    qDebug() << "regex:" << regex;

    const auto match = regex.match(comp_string);
    const auto has_match = match.hasMatch();

    return has_match;
}
