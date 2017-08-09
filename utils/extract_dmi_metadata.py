import sys
import json

from PIL import Image
from typing import Tuple, Union, List, Dict


def get_filename() -> str:
    try:
        return sys.argv[1]
    except KeyError:
        raise Exception("Dmi file param is missing!")


def get_file(file_name: str) -> Image:
    try:
        image = Image.open(file_name)
    except FileNotFoundError:
        raise Exception("No such file: {}".format(file_name))
    except OSError:
        raise Exception("Improper file format")
    return image


def is_metadata_valid(metadata: dict) -> bool:
    try:
        metadata = metadata['Description']
    except KeyError:
        return False
    if "BEGIN DMI" not in metadata or "END DMI" not in metadata:
        return False
    return True


def format_metadata(image: Image) -> List[str]:
    print("Extracting metadata from {}...".format(image.filename))

    if is_metadata_valid(image.info):
        metadata = image.info['Description'].split('\n')
        metadata = [line.replace('\t', '') for line in metadata]
        return metadata[1:-2]  # Strip BEGIN DMI/END DMI and empty last line
    raise Exception("Dmi metadata is corrupted!")


def extract_key_value(line: str) -> Tuple[str, Union[List[float], int, float, str]]:
    key = line.split()[0]
    value = line.split()[2]

    if key == 'state':
        value = value[1:-1]  # Strip quote symbols
        return key, value

    if ',' in value:
        value = map(float, value.split(','))
    else:
        try:
            value = int(value)
        except ValueError:
            value = float(value)
    return key, value


def get_file_info(metadata: List[str]) -> Dict:
    result = {
        'info': {},
        'states': []
    }

    current_object = result["info"]

    for line in metadata:
        key, value = extract_key_value(line)
        if key == "state":
            current_object = {}
            result["states"].append(current_object)
            current_object["state"] = value
            continue

        if type(value) in [list, int, float]:
            current_object[key] = value
            continue
    return result


def metadata_to_json(metadata: Dict) -> str:
    try:
        json_data = json.JSONEncoder(indent=4).encode(metadata)
        return json_data
    except json.JSONDecodeError:
        raise Exception("Dmi metadata is corrupted!")


file = get_file(get_filename())
data = get_file_info(format_metadata(file))
json_file_name = get_filename() + ".json"

with open(json_file_name, "w") as json_file:
    json_file.write(metadata_to_json(data))

print("Metadata was successfully written to '" + json_file_name + "'")
