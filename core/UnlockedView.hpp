// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef UNLOCKEDVIEW_HPP
#define UNLOCKEDVIEW_HPP

#include "chip_type.h"
#include "IView.hpp"

class UnlockedView : public TView<UnlockedView> {
public:
    UnlockedView(IViewDelegate<UnlockedView> *delegate);

    void setTransitionProgress(float progress) override;

    view_type type() const override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setChip(chip_type type);

    void setSuperchargerUnlocked();

    void setHeartsUnlocked();

private:
    bool _show_store_button = true;
    bool _show_hearts = false;
};

#endif
