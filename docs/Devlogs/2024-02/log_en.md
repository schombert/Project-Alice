# Progress as of Febraury 2024

It's San Valentines day, and we'll bring love and compassion to the otherwise not-so-loveful and compassionate victorian era.

## 1.0.9

Version [1.0.9](https://github.com/schombert/Project-Alice/releases/download/v1.0.9/1.0.9.7z) is available to download now!

## Modding extensions & the economy

This month was mainly dedicated to reworking the economy, thanks to one of our developers, peter, we had atleast two iterations from the original economic model, while I'm not certain how it works in detail, it supposedly has more dynamic behaviour to it, removes price caps and it's overall more "realistic".

Another particular change through the month was supporting non-AVX2 CPUs. While this may seem like a rather small change, a tiny portion of our userbase wasn't able to play Alice properly on older computers. However, our vectorization engine, DataContainer did in fact, support SSE4.2, SSE3 and AVX2, but it required some tweaks to make it properly work on real computers.

Since images are worth a thousand words, and I don't feel like writing an essay, here is a summary of new additions:

**Black map fonts**: After many complaints, we listened to you, and we added them. Access them via ESC -> Graphics Options -> Black map font

![black map fonts](./images/map.png)

**New event substitutions**: Give more flavour to your events and decisions, new substitutions such as `$party$`, `$government$`, `$culture_last_name$` and others will spice up your text - giving it more life to the events present at hand.

![party](./images/party.png)

**New mapmodes**: Religion, fort, issues, income, militancy, and many more.

![religion](./images/religion.png)

**New economy**: Dynamic prices, stabilization of demand/supply, elasticity and other neat economic properties.

![trade](./images/trade.png)

**General UI improvments**: Some bothering things people complained about have now been fixed, here for example, we expanded the controls for expanding factories.

![expand](./images/expand.png)

**Multiplayer improvments**: Fixed various OOS bugs (reminder that most OOS bugs occur NOT due to floating points, but rather due to multithreading). And also made the chat always visible!

![chat](./images/chat.png)

## AHMP

One of our developers, Nivaturimika, forked HPM and started doing some changes to it, to retrofit it for the new Alice engine. Some gameplay changes came with it too, such as being able to research any technology at startdate (provided you research the ones from the columns first). New substate mechanics - and general orientation towards catering to the multiplayer crowd. While realizing that Alice differs from the original game.

You can obtain AHMP here: [Github Repository](https://github.com/nivaturimika/ahmp)
