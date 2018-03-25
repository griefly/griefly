import argparse

def main():
    args_parser = argparse.ArgumentParser(description="Fix mapgen file after objects variables changes.")
    args_parser.add_argument("-a", "--action", help="Possible values: 'rename' & 'remove'.", required=True)

if __name__ == "__main__":
    main()
