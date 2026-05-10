#pragma once
#include <memory>
#include <functional>
#include <string>
#include "domain/repositories/IDailyLogRepository.hpp"
#include "domain/repositories/IMealEntryRepository.hpp"

namespace application {

struct DailyLogWithEntries {
    domain::DailyLog              log;
    std::vector<domain::MealEntry> entries;
};

class GetDailyLogUseCase : public std::enable_shared_from_this<GetDailyLogUseCase> {
    std::shared_ptr<domain::IDailyLogRepository>   logRepo_;
    std::shared_ptr<domain::IMealEntryRepository>  entryRepo_;
public:
    GetDailyLogUseCase(
        std::shared_ptr<domain::IDailyLogRepository>  logRepo,
        std::shared_ptr<domain::IMealEntryRepository> entryRepo)
        : logRepo_(std::move(logRepo))
        , entryRepo_(std::move(entryRepo))
    {}

    void execute(
        int64_t userId, const std::string& date,
        std::function<void(DailyLogWithEntries)> onSuccess,
        std::function<void(std::string)>         onError)
    {
        auto self         = shared_from_this();
        auto onSuccessPtr = std::make_shared<decltype(onSuccess)>(std::move(onSuccess));
        auto onErrorPtr   = std::make_shared<decltype(onError)>(std::move(onError));

        logRepo_->getOrCreate(userId, date,
            [self, onSuccessPtr, onErrorPtr](domain::DailyLog log) {
                self->entryRepo_->listByDailyLog(log.id,
                    [onSuccessPtr, log](std::vector<domain::MealEntry> entries) {
                        (*onSuccessPtr)({ log, std::move(entries) });
                    },
                    [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
            },
            [onErrorPtr](std::string err) { (*onErrorPtr)(std::move(err)); });
    }
};

} // namespace application
