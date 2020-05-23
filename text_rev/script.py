import pymorphy2
import numpy as np
import re


def get_text(file_path):
    morph = pymorphy2.MorphAnalyzer()
    try:
        with open(file_path, encoding='utf-8', errors='ignore') as f:
            url = f.readline()
            html = f.read()
            soup = BeautifulSoup(html, "html.parser")
            str_ = ''
            str_ = soup.text
            str_ = re.sub('\n', ' ', str_.lower())
            str_ = re.sub('[^a-zа-я0-9 ]', '', str_)
            str_ = [morph.parse(i)[0].normal_form for i in str_.split() if len(i) > 1 and len(i) < 20]
            str_ = ' '.join(str_)
            return url[:-1] + '\t' + str_
    except:
        print(file_path)
        return ''

def get_title(file_path):
    morph = pymorphy2.MorphAnalyzer()
    try:
        with open(file_path, encoding='utf-8', errors='ignore') as f:
            url = f.readline()
            html = f.read()
            soup = BeautifulSoup(html, "html.parser")
            str_ = ''
            str_ = soup.find('title').get_text
            str_ = re.sub('\n', ' ', str_.lower())
            str_ = re.sub('[^a-zа-я0-9 ]', '', str_)
            str_ = [morph.parse(i)[0].normal_form for i in str_.split() if len(i) > 1 and len(i) < 20]
            str_ = ' '.join(str_)
            return url[:-1] + '\t' + str_
    except:
        print(file_path)
        return ''


dir_names = ['20170708',  '20170710', '20170711', '20170717', '20170726']
for now_path in dir_names:
    print(now_path)
    cur_path = "D://sphere//textrev/content/content/" + now_path
    files = os.listdir(cur_path)
    files_path = list(map(lambda a: cur_path + '/' + a, files))
    with ThreadPool(4) as p:
        path_result = p.map(get_text, files_path)
    with open("D://sphere//textrev/text_norm/" + now_path, "w") as write_file:
        for i in path_result:
            write_file.write(i + '\n')


result = []
for now_path in dir_names:
    print(now_path)
    cur_path = "D://sphere//textrev/content/content/" + now_path
    files = os.listdir(cur_path)
    files_path = list(map(lambda a: cur_path + '/' + a, files))
    with ThreadPool(4) as p:
        path_result = p.map(get_title, files_path)
    result += path_result

with open("titles_norm.txt", "w") as f:
	for i in path_result:
		f.write(i + '\n')



urls_to_id = dict() # url -> id
id_to_urls=  dict() #id -> url
i= 0
with open("D:\\sphere\\textrev\\urls.numerate.txt", encoding='utf-8') as f:
    i = 0
    for s in f:
        tt = s.split('\t')
        id_ = int(tt[0])
        urls_to_id[tt[1][:-1]] = id_
        id_to_urls[id_] = tt[1][:-1]
        i += 1

quer_to_id = dict() #quer_num -> list_id
with open("D://sphere//textrev/sample.submission.txt", encoding='utf-8') as f:
    temp = f.readline()
    for line in f:
        q, ii = line.split(',')
        q = int(q)
        ii = int(ii[:-1])
        if q not in quer_to_id:
            quer_to_id[q] = []
        quer_to_id[q].append(ii)



# этот код востановленный, так что могут быть ошибки
url_to_text = dict()
files = os.listdir("D://sphere//textrev/text_norm")
for f in files:
	url, text = f.readline().split('\t')
	url_to_text[url] = text[:-1]

for q_num in quer_to_id.keys():
	with open("D://sphere//textrev/quer_norm/" + str(q_num), "w") as write_file: 
		need_id = quer_to_id[q_num]
		for id_ in need_id:
			id_url = id_to_urls[id_]
			write_file.write(id_url + '\t' + url_to_text[id_url] + '\n')

