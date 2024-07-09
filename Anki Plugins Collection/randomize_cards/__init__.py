from aqt import mw
from aqt.utils import showInfo
from aqt.qt import *
from anki.hooks import addHook
import random

class DeckChooserDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Choose Deck")
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.deck_combo = QComboBox()
        self.deck_combo.addItems([d['name'] for d in mw.col.decks.all()])
        self.layout.addWidget(self.deck_combo)

        self.button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        self.button_box.accepted.connect(self.accept)
        self.button_box.rejected.connect(self.reject)
        self.layout.addWidget(self.button_box)

    def get_selected_deck(self):
        return self.deck_combo.currentText()

def randomize_due_dates(deck_id):
    deck = mw.col.decks.get(deck_id)
    if not deck:
        showInfo("Deck not found.")
        return

    cards = mw.col.findCards(f"deck:'{deck['name']}'")
    if not cards:
        showInfo("No cards found in the deck.")
        return

    due_dates = [mw.col.getCard(card_id).due for card_id in cards]
    random.shuffle(due_dates)

    mw.col.modSchema(check=True)
    for card_id, new_due in zip(cards, due_dates):
        card = mw.col.getCard(card_id)
        card.due = new_due
        card.flush()

    mw.col.reset()
    showInfo(f"Due dates randomized for {len(cards)} cards in the deck '{deck['name']}'.")

def on_randomize_due():
    dialog = DeckChooserDialog(mw)
    if dialog.exec_():
        selected_deck_name = dialog.get_selected_deck()
        deck_id = mw.col.decks.id(selected_deck_name)
        randomize_due_dates(deck_id)

action = QAction("Randomize Due Dates", mw)
action.triggered.connect(on_randomize_due)
mw.form.menuTools.addAction(action)