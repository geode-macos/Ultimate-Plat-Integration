#include <Geode/modify/LevelInfoLayer.hpp>
#include <fstream>
#include <vector>

using namespace geode::prelude;

class $modify(PDCILevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        std::string message = "message";
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!level->isPlatformer()) return true;

        cocos2d::CCSprite* tierSprite = CCSprite::create("tier_2.png"_spr);
        tierSprite->setScale(0.375);

        
        cocos2d::CCMenu* tier = cocos2d::CCMenu::create();
        CCMenuItemSpriteExtra* tierButton = CCMenuItemSpriteExtra::create(
            tierSprite, this,
            menu_selector(PDCILevelInfoLayer::onButton)
        );
        tier->setID("PdcTier");
        tier->addChild(tierButton);
        tier->setPosition(135.0, 190.0);
        tier->setContentSize(cocos2d::CCSize());
        // tierButton->

        // auto leftSideMenu = this->getChildByID("left-side-menu");
        this->addChild(tier);
        // cocos2d::CCNode* gddl_label = this->getChildByID("b1rtek.gddlintegration/rating-menu");
        // if (gddl_label != nullptr)
        //     gddl_label->removeFromParent();

        return true;
    }
    void onButton(cocos2d::CCObject* sender) {
        // this->getChildByID("PdcTier")->removeFromParent();
        
        FLAlertLayer* flalertlayer = FLAlertLayer::create("Level Tier Info", m_fields->message, "Ok");

        flalertlayer->show();
    }
};