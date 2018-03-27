import argparse

def rename_variable(mapgen, oldname, newname):
    return;

def remove_variable(mapgen, name):
    return;

def main():
    args_parser = argparse.ArgumentParser(description="Fix mapgen file after objects variables changes.")
    args_parser.add_argument("-m", "--mapgen", help="path to mapgen.", required=True, nargs=1)
    args_parser.add_argument("-n", "--rename", help="rename variable.", nargs=2, metavar=("OLDNAME", "NEWNAME"))
    args_parser.add_argument("-r", "--remove", help="remove variable.", nargs=1)

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
