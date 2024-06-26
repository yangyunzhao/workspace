import os
from aqt import mw
from aqt.qt import *
from aqt.utils import showInfo
from anki.utils import intTime
import traceback

LOG_FILE = "D:\\123.log"

def log_message(message):
    with open(LOG_FILE, "a", encoding="utf-8") as f:
        f.write(f"{message}\n")

def clear_log():
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)

def delay_cards():
    try:
        log_message("开始延期卡片操作")
        
        decks = mw.col.decks.all()
        log_message(f"获取到 {len(decks)} 个牌组")
        
        deck_dialog = QDialog(mw)
        deck_dialog.setWindowTitle("选择要延期的牌组")
        layout = QVBoxLayout()
        deck_combo = QComboBox()
        deck_combo.addItems([d['name'] for d in decks])
        layout.addWidget(deck_combo)
        
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(deck_dialog.accept)
        button_box.rejected.connect(deck_dialog.reject)
        layout.addWidget(button_box)
        
        deck_dialog.setLayout(layout)
        
        if deck_dialog.exec_():
            selected_deck_name = deck_combo.currentText()
            selected_deck_id = next(d['id'] for d in decks if d['name'] == selected_deck_name)
            log_message(f"选择的牌组: {selected_deck_name}")
            
            cards = mw.col.findCards(f"deck:{selected_deck_name}")
            log_message(f"找到 {len(cards)} 张卡片")
            
            delayed_cards = 0
            for card_id in cards:
                card = mw.col.getCard(card_id)
                # 检查卡片是否为复习卡片（非新卡片）
                if card.type != 0:  # 0 表示新卡片
                    card.due = card.due + 1
                    card.mod = intTime()
                    card.flush()
                    delayed_cards += 1
            
            mw.col.save()
            
            log_message(f"已将 {selected_deck_name} 中的 {delayed_cards} 张复习卡片延期一天")
            showInfo(f"已将 {selected_deck_name} 中的 {delayed_cards} 张复习卡片延期一天。")
    except Exception as e:
        error_msg = f"错误: {str(e)}\n{traceback.format_exc()}"
        log_message(error_msg)
        showInfo(f"发生错误，详情请查看日志文件: {LOG_FILE}")

action = QAction("延期牌组", mw)
action.triggered.connect(delay_cards)
mw.form.menuTools.addAction(action)

clear_log()
log_message("插件已加载")