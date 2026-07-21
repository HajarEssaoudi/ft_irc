#!/usr/bin/env python3
"""
ft_irc channel tester
=====================

A small scripted IRC "client swarm" that connects several fake users to your
ft_irc server and runs them through a scenario that exercises JOIN, PART,
TOPIC, INVITE, KICK and MODE (i / t / k / o / l).

Usage:
    python3 test_irc_channels.py <port> <password> [host]

Example:
    python3 test_irc_channels.py 6667 mypass
    python3 test_irc_channels.py 6667 mypass 127.0.0.1

What it does, step by step (also printed as [STEP] markers):
    1. Connects alice, bob, charlie (PASS/NICK/USER for each)
    2. alice JOINs #test        -> alice becomes op (server should auto-op creator)
    3. alice sets TOPIC
    4. bob JOINs #test          -> should succeed (no restrictions yet)
    5. alice sets MODE #test +t -> topic restricted to ops
    6. bob tries TOPIC change   -> should be REJECTED (not op)
    7. alice sets MODE #test +i -> invite only
    8. charlie tries JOIN       -> should be REJECTED (not invited)
    9. alice INVITEs charlie
   10. charlie JOINs            -> should succeed now
   11. alice sets MODE #test +k secretkey
   12. bob PARTs, then tries to rejoin WITHOUT key -> should be REJECTED
   13. bob rejoins WITH key    -> should succeed
   14. alice sets MODE #test +l 3
   15. a 4th client (dave) tries to join a full channel -> should be REJECTED
   16. alice KICKs bob
   17. Final MODE #test query / summary of raw lines received per client

All raw lines received by every client are printed live, prefixed with the
nickname, so you can visually confirm the numeric replies / errors your
server sends match the RFC (e.g. 443, 471, 473, 475, 482, JOIN broadcast, etc).

No external dependencies - just sockets + threading from the stdlib.
"""

import socket
import sys
import threading
import time

READ_TIMEOUT = 0.3     # seconds to wait for more data on each drain
STEP_PAUSE = 0.4        # pause between scripted steps so output stays readable


class IRCClient:
    def __init__(self, host, port, nick, name_for_log=None):
        self.nick = nick
        self.log_name = name_for_log or nick
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        self.sock.settimeout(READ_TIMEOUT)
        self._buf = ""
        self._lock = threading.Lock()
        self._stop = False
        self._history = []
        self._thread = threading.Thread(target=self._reader_loop, daemon=True)
        self._thread.start()

    def _reader_loop(self):
        while not self._stop:
            try:
                data = self.sock.recv(4096)
                if not data:
                    break
                text = data.decode(errors="replace")
                with self._lock:
                    self._buf += text
                for line in text.split("\r\n"):
                    line = line.strip("\r\n")
                    if line:
                        self._history.append(line)
                        print(f"[{self.log_name:8s}] <-- {line}")
            except socket.timeout:
                continue
            except OSError:
                break

    def send(self, line):
        print(f"[{self.log_name:8s}] --> {line}")
        self.sock.sendall((line + "\r\n").encode())

    def close(self):
        self._stop = True
        try:
            self.sock.close()
        except OSError:
            pass

    def history_contains(self, needle):
        return any(needle in l for l in self._history)


def step(msg):
    print(f"\n[STEP] {msg}")
    time.sleep(STEP_PAUSE)


def register(client, password, username, realname):
    client.send(f"PASS {password}")
    client.send(f"NICK {client.nick}")
    client.send(f"USER {username} 0 * :{realname}")
    time.sleep(STEP_PAUSE)


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <port> <password> [host]")
        sys.exit(1)

    port = int(sys.argv[1])
    password = sys.argv[2]
    host = sys.argv[3] if len(sys.argv) > 3 else "127.0.0.1"

    clients = {}

    step("Connecting alice, bob, charlie and registering (PASS/NICK/USER)")
    alice = IRCClient(host, port, "alice")
    bob = IRCClient(host, port, "bob")
    charlie = IRCClient(host, port, "charlie")
    clients.update(alice=alice, bob=bob, charlie=charlie)

    register(alice, password, "alice", "Alice A")
    register(bob, password, "bob", "Bob B")
    register(charlie, password, "charlie", "Charlie C")

    step("alice JOINs #test (should become channel operator)")
    alice.send("JOIN #test")
    time.sleep(STEP_PAUSE)

    step("alice sets TOPIC")
    alice.send("TOPIC #test :Welcome to the test channel")
    time.sleep(STEP_PAUSE)

    step("bob JOINs #test (no restrictions yet -> should succeed)")
    bob.send("JOIN #test")
    time.sleep(STEP_PAUSE)

    step("alice sets MODE #test +t (topic restricted to ops)")
    alice.send("MODE #test +t")
    time.sleep(STEP_PAUSE)

    step("bob (not op) tries to change TOPIC -> should be REJECTED (482)")
    bob.send("TOPIC #test :bob was here")
    time.sleep(STEP_PAUSE)

    step("alice sets MODE #test +i (invite only)")
    alice.send("MODE #test +i")
    time.sleep(STEP_PAUSE)

    step("charlie (not invited) tries to JOIN #test -> should be REJECTED (473)")
    charlie.send("JOIN #test")
    time.sleep(STEP_PAUSE)

    step("alice INVITEs charlie")
    alice.send("INVITE charlie #test")
    time.sleep(STEP_PAUSE)

    step("charlie JOINs #test again -> should succeed now")
    charlie.send("JOIN #test")
    time.sleep(STEP_PAUSE)

    step("alice sets MODE #test +k secretkey")
    alice.send("MODE #test +k secretkey")
    time.sleep(STEP_PAUSE)

    step("bob PARTs #test")
    bob.send("PART #test :taking a break")
    time.sleep(STEP_PAUSE)

    step("bob tries to rejoin WITHOUT key -> should be REJECTED (475)")
    bob.send("JOIN #test")
    time.sleep(STEP_PAUSE)

    step("bob rejoins WITH correct key -> should succeed")
    bob.send("JOIN #test secretkey")
    time.sleep(STEP_PAUSE)

    step("alice sets MODE #test +l 3 (limit = current members)")
    alice.send("MODE #test +l 3")
    time.sleep(STEP_PAUSE)

    step("dave connects and tries to JOIN full channel -> should be REJECTED (471)")
    dave = IRCClient(host, port, "dave")
    clients["dave"] = dave
    register(dave, password, "dave", "Dave D")
    dave.send("JOIN #test secretkey")
    time.sleep(STEP_PAUSE)

    step("alice KICKs bob")
    alice.send("KICK #test bob :bye bob")
    time.sleep(STEP_PAUSE)

    step("Done. Sending QUIT to all clients.")
    for c in clients.values():
        c.send("QUIT :test finished")
    time.sleep(STEP_PAUSE)

    for c in clients.values():
        c.close()

    print("\n[SUMMARY] Quick sanity checks based on captured replies:")
    checks = [
        ("bob got 482 (not op) on topic attempt", bob, "482"),
        ("charlie got 473 (invite only) on first join", charlie, "473"),
        ("bob got 475 (bad channel key) before using key", bob, "475"),
        ("dave got 471 (channel full)", dave, "471"),
    ]
    for desc, client, code in checks:
        ok = client.history_contains(code)
        print(f"  [{'OK' if ok else 'CHECK MANUALLY'}] {desc} -> saw '{code}': {ok}")

    print("\nReview the [STEP]/<-- output above line by line against the RFC "
          "numerics your server should send. Numeric codes above are the "
          "standard IRC ones; adjust the checks if your implementation uses "
          "different codes.")


if __name__ == "__main__":
    main()