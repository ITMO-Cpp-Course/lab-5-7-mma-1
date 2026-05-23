#pragma once

#include <expected>
#include <string>
#include <string_view>

namespace transaction
{
    enum class ErrorCode
    {
        Success = 0,
        InvalidDocumentId,
        EmptySearchQuery,
        EmptyDocumentContent,
        DocumentNotFound,
        DuplicateDocumentId,
        TransactionNotActive,
        TransactionCommitFailed,
        TransactionAlreadyCommitted,
        InternalError,
    };

    template <typename T>
    using Result = std::expected<T, ErrorCode>;

    inline std::string_view errorCodeToString(ErrorCode code) noexcept
    {
        using enum ErrorCode;

        switch (code)
        {
        case Success: return "Success";
        case InvalidDocumentId: return "Invalid document ID (must be > 0)";
        case EmptySearchQuery: return "Empty search query";
        case EmptyDocumentContent: return "Empty document content";
        case DocumentNotFound: return "Document not found";
        case DuplicateDocumentId: return "Document with this ID already exists";
        case TransactionNotActive: return "No active transaction";
        case TransactionCommitFailed: return "Failed to commit transaction";
        case TransactionAlreadyCommitted: return "Transaction already committed";
        case InternalError: return "Internal error occurred";
        default: return "Unknown error";
        }
    }

    inline std::string formatError(ErrorCode code, std::string_view context = {})
    {
        std::string result(errorCodeToString(code));
        if (!context.empty())
        {
            result += " [context: ";
            result += context;
            result += ']';
        }
        return result;
    }
}
