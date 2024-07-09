import tempfile
from aqt import mw
from aqt.qt import *
from aqt.utils import showInfo
import xml.etree.ElementTree as ET
import os
from aqt.progress import ProgressManager

LOG_FILE_PATH = "D:\\123.log"

def log_message(message):
    with open(LOG_FILE_PATH, "a", encoding='utf-8') as log_file:
        log_file.write(f"{message}\n")
    print(message)  # 同时在控制台输出，方便调试

def select_xml_file():
    file_path, _ = QFileDialog.getOpenFileName(mw, "选择XML文件", "", "XML Files (*.xml);;All Files (*)")
    if file_path:
        log_message(f"选择的文件: {file_path}")
        return file_path
    log_message("未选择文件")
    return None

def extract_words_from_xml(file_path):
    try:
        tree = ET.parse(file_path)
        root = tree.getroot()
        words = [item.attrib['word'] for item in root.findall(".//CustomizeListItem")]
        log_message(f"提取的单词数量: {len(words)}")
        log_message(f"提取的单词: {words}")
        return words
    except Exception as e:
        log_message(f"从XML提取单词时出错: {str(e)}")
        return []

def save_words_to_txt(words):
    try:
        with tempfile.NamedTemporaryFile(mode='w+', encoding='utf-8', suffix='.txt', delete=False) as temp_file:
            for word in words:
                temp_file.write(f"{word}\n")  # 只写入单词，不添加额外的制表符
        log_message(f"单词已临时保存到: {temp_file.name}")
        
        return temp_file.name
    except Exception as e:
        log_message(f"保存单词到临时文件时出错: {str(e)}")
        return None

def import_to_anki(file_path):
    try:
        deck_name = "导入"
        log_message(f"尝试导入到牌组: {deck_name}")
        did = mw.col.decks.id(deck_name)
        log_message(f"牌组ID: {did}")
        mw.col.decks.select(did)
        
        model_name = "英文-英文听写"
        log_message(f"尝试使用笔记类型: {model_name}")
        model = mw.col.models.byName(model_name)
        if not model:
            log_message(f"未找到'{model_name}'笔记类型")
            return 0, 0
        log_message(f"笔记类型字段: {[f['name'] for f in model['flds']]}")
        
        mw.col.models.setCurrent(model)
        
        log_message(f"开始导入过程，文件路径: {file_path}")
        imported_count = 0
        skipped_count = 0

        # 获取总单词数
        with open(file_path, 'r', encoding='utf-8') as f:
            total_words = sum(1 for _ in f)

        progress = ProgressManager(mw)
        progress.start(max=total_words, min=0, label="导入单词中...")

        with open(file_path, 'r', encoding='utf-8') as f:
            for i, line in enumerate(f):
                word = line.strip()
                
                progress.update(value=i, label=f"正在导入: {word}")
                
                # 检查单词是否已存在
                existing_notes = mw.col.findNotes(f'"单词:{word}"')
                if existing_notes:
                    log_message(f"跳过已存在的单词: {word}")
                    skipped_count += 1
                    continue
                
                note = mw.col.newNote()
                note.model()['did'] = did  # 明确设置牌组ID
                for i in range(len(model['flds'])):
                    note.fields[i] = word  # 将单词复制到所有8个字段
                try:
                    mw.col.addNote(note)
                    imported_count += 1
                    log_message(f"成功添加单词: {word}")
                except Exception as e:
                    log_message(f"添加单词 '{word}' 失败: {str(e)}")
                
                # 检查笔记是否被添加到正确的牌组
                if note.id:
                    note_deck = mw.col.decks.name(note.cards()[0].did)
                    if note_deck != deck_name:
                        log_message(f"警告：单词 '{word}' 被添加到了错误的牌组: {note_deck}")
        
        progress.finish()
        mw.col.save()  # 保存更改
        
        os.unlink(file_path)
        log_message(f"临时文件已删除: {file_path}")
        
        log_message(f"成功导入 {imported_count} 个单词到 '{deck_name}' 牌组")
        log_message(f"跳过 {skipped_count} 个已存在的单词")
        return imported_count, skipped_count
    except Exception as e:
        log_message(f"导入到Anki时出错: {str(e)}")
        import traceback
        log_message(f"错误追踪:\n{traceback.format_exc()}")
        return 0, 0

def on_import_xml():
    try:
        xml_file_path = select_xml_file()
        if xml_file_path:
            words = extract_words_from_xml(xml_file_path)
            if words:
                temp_file_path = save_words_to_txt(words)
                if temp_file_path:
                    imported_count, skipped_count = import_to_anki(temp_file_path)
                    message = f"已成功导入 {imported_count} 个单词到 '导入' 牌组。\n跳过 {skipped_count} 个已存在的单词。"
                    log_message(message)
                    showInfo(message)
                else:
                    showInfo("保存单词到临时文件时出错，请查看日志。")
            else:
                showInfo("从XML提取到的单词为空")
        else:
            showInfo("未选择文件")
    except Exception as e:
        log_message(f"意外错误: {str(e)}")
        import traceback
        log_message(f"错误追踪:\n{traceback.format_exc()}")
        showInfo(f"发生错误: {str(e)}")

action = QAction("从欧陆词典导入单词", mw)
action.triggered.connect(on_import_xml)
mw.form.menuTools.addAction(action)

log_message("Anki插件已加载")