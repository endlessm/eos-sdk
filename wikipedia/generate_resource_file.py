import json
import xml.etree.ElementTree as ET

path = "../data/app_info"
filename = os.listdir(path)[0]
handle = open(filename, 'r')
app_info = json.loads(handle.read())
categories = app_info['categories']
app_image_name = "app_image/" + app_info['app_image']

tree = ET.parse('..data/endless_brazil.gresource.xml')
root = tree.getroot()
filenode = root.find('gresource')

for category in categories:
	image_file = category['image_file']
	new_file_name = "category_images/" + image_file
	elem = ET.Element('file')
	elem.text = new_file_name
	filenode.append(elem)
ET.dump(root)