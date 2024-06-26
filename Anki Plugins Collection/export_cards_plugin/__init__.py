import json
import os
from aqt import mw
from aqt.qt import QAction, QFileDialog, QInputDialog

LOG_FILE = "D:\\123.log"
CONFIG_DECK_NAME = "英语单词"  # 配置的deck名字
DEFAULT_SAVE_PATH = "C:\\Users\\ZNMLR\\Desktop\\exported_cards.txt"  # 默认保存路径

def log_to_file(message):
    with open(LOG_FILE, "a", encoding="utf-8") as log_file:
        log_file.write(message + "\n")
        
def clear_log():
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)

def export_due_cards():
    try:
        log_to_file("Starting export_due_cards function")
        
        # 获取当前的Anki集合
        collection = mw.col
        log_to_file("Collection loaded")
        
        # 获取所有deck的名字
        deck_names = collection.decks.allNames()
        log_to_file(f"Deck names: {deck_names}")
        
        # 选择deck
        if CONFIG_DECK_NAME and CONFIG_DECK_NAME in deck_names:
            deck_name = CONFIG_DECK_NAME
        else:
            deck_name, ok = QInputDialog.getItem(mw, "Select Deck", "Choose a deck:", deck_names, 0, False)
            if not ok or not deck_name:
                log_to_file("No deck selected.")
                return
        
        # 获取deck ID
        deck_id = collection.decks.id(deck_name)
        log_to_file(f"Selected Deck ID: {deck_id}")
        
        # 获取当天需要复习的卡片，包括新卡片和到期卡片
        today = collection.sched.today
        due_cards = collection.db.list(f"SELECT id FROM cards WHERE did = {deck_id} AND queue IN (0, 2, 1, 3) AND due <= ?", today)
        log_to_file(f"Due Cards Query: SELECT id FROM cards WHERE did = {deck_id} AND queue IN (0, 2, 1, 3) AND due <= {today}")
        log_to_file(f"Due Cards: {due_cards}")
        
        # 如果今天没有到期卡片，检查明天的到期卡片
        if not due_cards:
            log_to_file("No due cards found for today. Checking for tomorrow.")
            tomorrow = today + 1
            due_cards = collection.db.list(f"SELECT id FROM cards WHERE did = {deck_id} AND queue IN (0, 2, 1, 3) AND due <= ?", tomorrow)
            log_to_file(f"Due Cards Query for tomorrow: SELECT id FROM cards WHERE did = {deck_id} AND queue IN (0, 2, 1, 3) AND due <= {tomorrow}")
            log_to_file(f"Due Cards for tomorrow: {due_cards}")
        
        # 获取新卡片，按配置中的“New cards/day”限制数量
        new_cards_per_day = collection.decks.confForDid(deck_id)['new']['perDay']
        new_cards = collection.db.list(f"SELECT id FROM cards WHERE did = {deck_id} AND queue = 0 ORDER BY due LIMIT ?", new_cards_per_day)
        log_to_file(f"New Cards Query: SELECT id FROM cards WHERE did = {deck_id} AND queue = 0 ORDER BY due LIMIT {new_cards_per_day}")
        log_to_file(f"New Cards: {new_cards}")

        # 如果没有卡片需要复习，提示用户
        if not due_cards and not new_cards:
            log_to_file("No cards due today or tomorrow.")
            return

        # 导出卡片的key字段（假设我们选择字段 '单词' 作为 key 字段）
        review_cards = []
        for card_id in due_cards:
            card = collection.getCard(card_id)
            note = card.note()
            if '单词' in note:
                key_field = note['单词']
                review_cards.append(key_field)
                log_to_file(f"Review Card ID {card_id}: {key_field}")
            else:
                log_to_file(f"Card ID {card_id} skipped, '单词' field not found.")
        
        # 按长度排序复习卡片
        review_cards.sort(key=len)
        log_to_file(f"Sorted Review Cards: {review_cards}")

        new_cards_list = []
        for card_id in new_cards:
            card = collection.getCard(card_id)
            note = card.note()
            if '单词' in note:
                key_field = note['单词']
                new_cards_list.append(key_field)
                log_to_file(f"New Card ID {card_id}: {key_field}")
            else:
                log_to_file(f"Card ID {card_id} skipped, '单词' field not found.")

        log_to_file(f"New Cards List: {new_cards_list}")

        # 保存到文件
        if DEFAULT_SAVE_PATH:
            file_path = DEFAULT_SAVE_PATH
        else:
            file_path, _ = QFileDialog.getSaveFileName(mw, "Save Exported Cards", "", "Text Files (*.txt)")
        
        if file_path:
            with open(file_path, 'w', encoding='utf-8') as f:
                # 写入复习的卡片
                for card in review_cards:
                    f.write(card + "\n")
                # 写入分隔符
                f.write("=======\n")
                # 写入新增的卡片
                for card in new_cards_list:
                    f.write(card + "\n")
            log_to_file("Exported successfully!")

    except Exception as e:
        log_to_file(f"An error occurred: {e}")

# 添加菜单项
action = QAction("导出牌组卡片", mw)
action.triggered.connect(export_due_cards)
mw.form.menuTools.addAction(action)

# 在Anki启动时清空日志
clear_log()