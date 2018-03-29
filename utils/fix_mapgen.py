import argparse
import json

def import_mapgen(filename):
    with open(filename, "r") as json_file:
        raw_json = json_file.read()

    mapgen = json.JSONDecoder().decode(raw_json)
    return mapgen

def rename_variable(filename, oldname, newname):
    mapgen = import_mapgen(filename)
    return;

def remove_variable(filename, name):
    mapgen = import_mapgen(filename)
    return;

def main():
    args_parser = argparse.ArgumentParser(description="Fix mapgen file after objects variables changes.")
    args_parser.add_argument("-m", "--mapgen", help="path to mapgen.", required=True)
    args_parser.add_argument("-n", "--rename", help="rename variable.", nargs=2, metavar=("OLDNAME", "NEWNAME"))
    args_parser.add_argument("-r", "--remove", help="remove variable.")

    args = args_parser.parse_args()

    if args.rename and args.remove:
        print("Rename xor remove!")
    if not args.rename and not args.remove:
        args_parser.print_help()
        return

    if args.rename:
        rename_variable(args.mapgen, args.rename[0], args.rename[1])
    else:
        remove_variable(args.mapgen, args.remove)

if __name__ == "__main__":
    main()
