// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef STOREHANDLER_HPP
#define STOREHANDLER_HPP

#include "assets.hpp"
#include "color.hpp"
#include "IGameHandler.hpp"
#include "in_app_purchase_type.h"

class SwipeHandlerDelegate {
public:
    virtual void changePage(short page_delta) = 0;

    virtual bool isFirstPage() const = 0;

    virtual bool isLastPage() const = 0;
};

class StoreHandler : public GameStateHandler<app_state::store>, public SwipeHandlerDelegate {
public:
    using GameStateHandler<app_state::store>::GameStateHandler;

    ~StoreHandler();

    void init(class IStoreHandlerDelegate *delegate, unsigned short goal);

    void update(float t) override;

    void touchesBegan(const vec2<double> &p, long long t_ms) override;

    void touchesMoved(const vec2<double> &p, long long t_ms) override;

    void touchesEnded(const vec2<double> &p, long long t_ms) override;

    void willBecomeActive() override;

    void willResignActive() override;

    bool collisionEnabled() const override;

    void setTransitionProgress(float progress);

    void startExitTransition();

    void goalCompleted(unsigned short goal);

    void setGoal(unsigned short goal);

    void setPrice(in_app_purchase_type type, const std::wstring &price);

    void addProgress(unsigned long long progress);

    unsigned short chipLevel() const;

    chip_type unlockedChip() const;

    chip_type nextUnlockableChip() const;

    bool isTransitionRunning() const;

    bool canShowGoals() const;

    std::wstring price(in_app_purchase_type type) const;

    unsigned short goalsToUnlockNextChip() const;

    unsigned short currentLevel(chip_type type) const;

    unsigned long pointsRequired(chip_type type) const;

    unsigned long progress(chip_type type) const;

private:
    void updateTransitionState(unsigned short i0, unsigned short i1, float t);

    void updateCurrentlyDisplayedPage(unsigned short i0, unsigned short i1, float t);

    void setCurrentlyDisplayedPage(unsigned short currently_displayed_page);

    void addChipForTransition(chip_type type);

    // MARK: - SwipeHandlerDelegate
    void changePage(short page_delta) override;

    bool isFirstPage() const override;

    bool isLastPage() const override;

private:
    struct {
        unsigned long  points_required[2];
        unsigned short goals_required;
    } _chips_progress[chip_type_range];

    class IStoreHandlerDelegate *_delegate = nullptr;
    class SwipeHandler          *_swipe_handler = nullptr;

    std::vector<Chip>            _initial_chips_data;
    std::wstring                 _prices[in_app_purchase_type_range];
    vec2<float>                  _chips_offset;
    chip_type                    _unlocked_chip = chip_type_range;

    unsigned short               _current_levels[chip_type_range];
    unsigned short               _page_index = 0;
    unsigned short               _currently_displayed_page = 0;
    unsigned short               _focused_chip_index = 0;
    unsigned short               _initial_last_index;
    unsigned short               _goals_to_unlock_next_chip = assets::goals_count;
    bool                         _buy_animation_running = false;
    bool                         _interaction_allowed = false;
};

#endif
