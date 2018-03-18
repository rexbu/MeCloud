import random

seed = "23456789ABCDEFGHJKMNPQRSTUVWXYZ"


def create_code():
    sa = []
    for i in range(4):
        sa.append(random.choice(seed))
    salt = ''.join(sa)
    return salt


if __name__ == '__main__':
    print create_code()
