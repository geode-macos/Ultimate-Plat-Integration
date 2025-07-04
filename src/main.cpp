#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>

#include <fstream>
#include <vector>
#include <limits>

using namespace geode::prelude;

// class CustomJson {  // I tried to find out how the original matjson worked but it was so complicated and buggy that I just decided to write my own cpp class
// public:
//     std::string id;
//     std::string name;
//     CustomJson() {}
//     CustomJson(std::string id, std::string name) : id(id), name(name) {}
//     static std::vector<CustomJson> parse(const matjson::Value& value) {
//         std::vector<CustomJson> result(0);
//         if (!value.contains("values")) {
//             return result;}
//         matjson::Value value1 = value["values"].asArray().unwrap();
//         log::debug("{}", value["values"][0][0].asString().unwrapOr("Couldn't unwrap"));
//         return result;
//     }
// };

$execute {
    // std::string url = "https://sheets.googleapis.com/v4/spreadsheets/1hQdyR7fafhN9hKRu-KHg036fhMXUmC3_fQix0qVQA6k/values/Main!A:F?key=AIzaSyAVpBERRc97KM9PNu_sv5q6BEQ7ccUlxac";
    // web::WebRequest req = web::WebRequest();
    // web::WebTask task = req.get(url);
    // EventLltask.getListener();
    // // Mod::get()->setSavedValue<Result<matjson::Value>>("msg", "Mod Saved Message");
    // Mod::get()->setSavedValue<std::string>("msg", "Mod Saved Message");
    // EventListener<web::WebTask> m_listener;

    // m_listener.bind([] (web::WebTask::Event* e) {
    //     if (web::WebResponse* res = e->getValue()) {
    //         log::debug("{}", res->json().unwrapOr("{}"));
    //     } else if (web::WebProgress* p = e->getProgress()) {
    //         log::debug("progress: {}", p->downloadProgress().value_or(0.f));
    //     } else if (e->isCancelled()) {
    //         log::debug("The request was cancelled... So sad :(");
    //     }
    // });

    // // auto req = web::WebRequest();
    // // Let's fetch... uhh...
    // m_listener.setFilter(req.get(url));
};

int pdc_find_level(matjson::Value pdc_levels, GJGameLevel* level) {
    log::debug("[{}", pdc_levels.size());
    for (int i = 2; i < pdc_levels.size(); i++) {
        log::debug("{}, {}", pdc_levels.operator[](i).operator[](0).asString().unwrap(), std::format("{}", level->m_levelID.value()));
        if (pdc_levels.operator[](i).operator[](0).asString().unwrap() == std::format("{}", level->m_levelID.value()))
            return i;
    }
    log::debug("Didn't find the ID");
    return -1;
};

CCSprite* generate_tiersprite(std::string s) {
    if (s == "1")  return CCSprite::create("tier_1.png"_spr);
    if (s == "2")  return CCSprite::create("tier_2.png"_spr);
    if (s == "3")  return CCSprite::create("tier_3.png"_spr);
    if (s == "4")  return CCSprite::create("tier_4.png"_spr);
    if (s == "5")  return CCSprite::create("tier_5.png"_spr);
    if (s == "6")  return CCSprite::create("tier_6.png"_spr);
    if (s == "7")  return CCSprite::create("tier_7.png"_spr);
    if (s == "8")  return CCSprite::create("tier_8.png"_spr);
    if (s == "9")  return CCSprite::create("tier_9.png"_spr);
    if (s == "10") return CCSprite::create("tier_10.png"_spr);
    if (s == "11") return CCSprite::create("tier_11.png"_spr);
    if (s == "12") return CCSprite::create("tier_12.png"_spr);
    if (s == "13") return CCSprite::create("tier_13.png"_spr);
    else           return CCSprite::create("tier_0.png"_spr);
};



class $modify(PDCILevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!level->isPlatformer()) return true;
        this->read_online_data();

        matjson::Value pdc_levels = Mod::get()->getSavedValue<matjson::Value>("levels");

        int pdc_inner_id = pdc_find_level(pdc_levels, level);
        
        // pdc_levels[pdc_inner_id][2]

        cocos2d::CCSprite* tierSprite;
        if (pdc_inner_id == -1)
            tierSprite = generate_tiersprite("0");
        else
            tierSprite = generate_tiersprite(pdc_levels[pdc_inner_id][2].asString().unwrapOr("2"));
        // cocos2d::CCSprite* tierSprite = CCSprite::create("tier_2.png"_spr);
        tierSprite->setScale(0.375);

        
        cocos2d::CCMenu* tier = cocos2d::CCMenu::create();
        CCMenuItemSpriteExtra* tierButton = CCMenuItemSpriteExtra::create(
            tierSprite, this,
            menu_selector(PDCILevelInfoLayer::onButton)
        );
        tier->setID("PdcTier");
        tier->addChild(tierButton);
        tier->setPosition(135.0, 190.0);
        // auto winSize = CCDirector::sharedDirector()->getWinSize();
        // tier->setPosition({winSize.width / 2 - 50, winSize.height / 2 - 50});
        tier->setContentSize(cocos2d::CCSize());
        // tier->setLayout(ColumnLayout::create()->setGap(5.0f)->setAxisReverse(true)->setAutoScale(true));
        // tierButton->

        // auto leftSideMenu = this->getChildByID("left-side-menu");
        this->addChild(tier);
        // cocos2d::CCNode* gddl_label = this->getChildByID("b1rtek.gddlintegration/rating-menu");
        // if (gddl_label != nullptr)
        //     gddl_label->removeFromParent();

        return true;
    };
    void read_online_data() {
        log::debug("Entered the read_online_data() function");
        this->m_fields->m_listener.bind([] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                log::debug("Got the value");
                matjson::Value savedvalue = res->json().unwrapOr(matjson::Value());
                if (!savedvalue.contains("values"))
                    Mod::get()->setSavedValue<matjson::Value>("levels", matjson::Value());
                else {
                    log::debug("  Entered here");
                    matjson::Value levels_ = savedvalue["values"];
                    // log::debug("{}", levels_[1][1].asString().unwrapOr(":("));
                    matjson::Value result = levels_;
                    // matjson::Value result = matjson::Value();
                    log::debug("{}", levels_.size());
                    // for (int i = 0; i < levels_.size(); i++) {
                    //     // result.push(matjson::Value({
                    //     //     matjson::Value(levels_[i][0]),
                    //     //     matjson::Value(levels_[i][1]),
                    //     //     matjson::Value(levels_[1][2])
                    //     // }));
                    //     result.push(levels_[i]);
                    // }
                    
                    Mod::get()->setSavedValue<matjson::Value>("levels", result);
                }
                // CustomJson::parse(res->json().unwrapOr(matjson::Value()));
                // Mod::get()->setSavedValue<std::string>("levels", res->string().unwrapOr("{}"));
                // log::debug("{}", res->string().unwrapOr("Uh oh!"));
                // log::debug("{}", Mod::get()->getSavedValue<Result<matjson::Value, matjson::ParseError>>("levels").ok());
                // log::debug("{}", Mod::get()->getSavedValue<Result<matjson::Value, matjson::ParseError>>("levels").err());
            } else if (web::WebProgress* p = e->getProgress()) {
                // log::debug("progress: {}", p->downloadProgress().value_or(0.f));
            } else if (e->isCancelled()) {
                log::debug("The request was cancelled... So sad :(");
            }
        });
        auto req = web::WebRequest();
        // Let's fetch... uhh...
        // std::string url = "https://sheets.googleapis.com/v4/spreadsheets/1hQdyR7fafhN9hKRu-KHg036fhMXUmC3_fQix0qVQA6k/values/'Raw'!A:P?key=AIzaSyAVpBERRc97KM9PNu_sv5q6BEQ7ccUlxac";
        std::string url = "https://sheets.googleapis.com/v4/spreadsheets/1ApwiAVAcBmfyoPW3wvDzc8JvY4Lfg5tFsPlYg3DNWhc/values/'RawData'!A:P?key=AIzaSyAVpBERRc97KM9PNu_sv5q6BEQ7ccUlxac";
        this->m_fields->m_listener.setFilter(req.get(url));
    };
    // void onButton(cocos2d::CCObject* sender) {
    //     // this->getChildByID("PdcTier")->removeFromParent();
    //     FLAlertLayer* flalertlayer = FLAlertLayer::create("Level Tier Info", "Empty message", "Ok");

    //     flalertlayer->show();
    //     // log::debug("{}", Mod::get()->getSavedValue<matjson::Value>("levels")[].asArray().unwrap());
    // }
};