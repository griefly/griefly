import argparse

def main():
    args_parser = argparse.ArgumentParser(description="Fix mapgen file after objects variables changes.")
    args_parser.add_argument("-n", "--rename", help="rename variable.", required=False, nargs=2, metavar=("OLDNAME", "NEWNAME"))
    args_parser.add_argument("-r", "--remove", help="remove variable.", required=False, nargs=1)

    args = args_parser.parse_args()

if __name__ == "__main__":
    main()
