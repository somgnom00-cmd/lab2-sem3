package structures

import "fmt"

type Array struct {
	data []string
}

func NewArray() *Array                               { return &Array{} }
func (a *Array) Type() string                        { return "Array" }
func (a *Array) Size() int                           { return len(a.data) }
func (a *Array) Values() []string                    { return append([]string(nil), a.data...) }
func (a *Array) Clear()                              { a.data = nil }
func (a *Array) PushBack(value string)               { a.data = append(a.data, value) }
func (a *Array) appendSerialized(value string) error { a.PushBack(value); return nil }

func (a *Array) Insert(index int, value string) error {
	if index < 0 || index > len(a.data) {
		return fmt.Errorf("array index out of range")
	}
	a.data = append(a.data, "")
	copy(a.data[index+1:], a.data[index:])
	a.data[index] = value
	return nil
}

func (a *Array) Erase(index int) error {
	if index < 0 || index >= len(a.data) {
		return fmt.Errorf("array index out of range")
	}
	copy(a.data[index:], a.data[index+1:])
	a.data = a.data[:len(a.data)-1]
	return nil
}

func (a *Array) At(index int) (string, error) {
	if index < 0 || index >= len(a.data) {
		return "", fmt.Errorf("array index out of range")
	}
	return a.data[index], nil
}

func (a *Array) Find(value string) int {
	for i, stored := range a.data {
		if stored == value {
			return i
		}
	}
	return -1
}

type singleNode struct {
	value string
	next  *singleNode
}

type SinglyList struct {
	head *singleNode
	tail *singleNode
	size int
}

func NewSinglyList() *SinglyList                          { return &SinglyList{} }
func (l *SinglyList) Type() string                        { return "SinglyList" }
func (l *SinglyList) Size() int                           { return l.size }
func (l *SinglyList) Clear()                              { l.head, l.tail, l.size = nil, nil, 0 }
func (l *SinglyList) appendSerialized(value string) error { l.PushBack(value); return nil }

func (l *SinglyList) PushFront(value string) {
	added := &singleNode{value: value, next: l.head}
	l.head = added
	if l.tail == nil {
		l.tail = added
	}
	l.size++
}

func (l *SinglyList) PushBack(value string) {
	added := &singleNode{value: value}
	if l.tail != nil {
		l.tail.next = added
	} else {
		l.head = added
	}
	l.tail = added
	l.size++
}

func (l *SinglyList) nodeAt(index int) (*singleNode, error) {
	if index < 0 || index >= l.size {
		return nil, fmt.Errorf("list index out of range")
	}
	node := l.head
	for i := 0; i < index; i++ {
		node = node.next
	}
	return node, nil
}

func (l *SinglyList) Insert(index int, value string) error {
	if index < 0 || index > l.size {
		return fmt.Errorf("list index out of range")
	}
	if index == 0 {
		l.PushFront(value)
		return nil
	}
	if index == l.size {
		l.PushBack(value)
		return nil
	}
	before, _ := l.nodeAt(index - 1)
	before.next = &singleNode{value: value, next: before.next}
	l.size++
	return nil
}

func (l *SinglyList) Erase(index int) error {
	if index < 0 || index >= l.size {
		return fmt.Errorf("list index out of range")
	}
	if index == 0 {
		l.head = l.head.next
		l.size--
		if l.size == 0 {
			l.tail = nil
		}
		return nil
	}
	before, _ := l.nodeAt(index - 1)
	if before.next == l.tail {
		l.tail = before
	}
	before.next = before.next.next
	l.size--
	return nil
}

func (l *SinglyList) Values() []string {
	result := make([]string, 0, l.size)
	for node := l.head; node != nil; node = node.next {
		result = append(result, node.value)
	}
	return result
}

type doubleNode struct {
	value string
	prev  *doubleNode
	next  *doubleNode
}

type DoublyList struct {
	head *doubleNode
	tail *doubleNode
	size int
}

func NewDoublyList() *DoublyList                          { return &DoublyList{} }
func (l *DoublyList) Type() string                        { return "DoublyList" }
func (l *DoublyList) Size() int                           { return l.size }
func (l *DoublyList) Clear()                              { l.head, l.tail, l.size = nil, nil, 0 }
func (l *DoublyList) appendSerialized(value string) error { l.PushBack(value); return nil }

func (l *DoublyList) PushFront(value string) {
	added := &doubleNode{value: value, next: l.head}
	if l.head != nil {
		l.head.prev = added
	} else {
		l.tail = added
	}
	l.head = added
	l.size++
}

func (l *DoublyList) PushBack(value string) {
	added := &doubleNode{value: value, prev: l.tail}
	if l.tail != nil {
		l.tail.next = added
	} else {
		l.head = added
	}
	l.tail = added
	l.size++
}

func (l *DoublyList) nodeAt(index int) (*doubleNode, error) {
	if index < 0 || index >= l.size {
		return nil, fmt.Errorf("list index out of range")
	}
	if index < l.size/2 {
		node := l.head
		for i := 0; i < index; i++ {
			node = node.next
		}
		return node, nil
	}
	node := l.tail
	for i := l.size - 1; i > index; i-- {
		node = node.prev
	}
	return node, nil
}

func (l *DoublyList) Insert(index int, value string) error {
	if index < 0 || index > l.size {
		return fmt.Errorf("list index out of range")
	}
	if index == 0 {
		l.PushFront(value)
		return nil
	}
	if index == l.size {
		l.PushBack(value)
		return nil
	}
	current, _ := l.nodeAt(index)
	added := &doubleNode{value: value, prev: current.prev, next: current}
	current.prev.next = added
	current.prev = added
	l.size++
	return nil
}

func (l *DoublyList) Erase(index int) error {
	node, err := l.nodeAt(index)
	if err != nil {
		return err
	}
	if node.prev != nil {
		node.prev.next = node.next
	} else {
		l.head = node.next
	}
	if node.next != nil {
		node.next.prev = node.prev
	} else {
		l.tail = node.prev
	}
	l.size--
	return nil
}

func (l *DoublyList) Values() []string {
	result := make([]string, 0, l.size)
	for node := l.head; node != nil; node = node.next {
		result = append(result, node.value)
	}
	return result
}

func (l *DoublyList) ReverseValues() []string {
	result := make([]string, 0, l.size)
	for node := l.tail; node != nil; node = node.prev {
		result = append(result, node.value)
	}
	return result
}

type Stack struct {
	top  *singleNode
	size int
}

func NewStack() *Stack             { return &Stack{} }
func (s *Stack) Type() string      { return "Stack" }
func (s *Stack) Size() int         { return s.size }
func (s *Stack) Clear()            { s.top, s.size = nil, 0 }
func (s *Stack) Push(value string) { s.top = &singleNode{value: value, next: s.top}; s.size++ }

func (s *Stack) appendSerialized(value string) error {
	added := &singleNode{value: value}
	if s.top == nil {
		s.top = added
	} else {
		bottom := s.top
		for bottom.next != nil {
			bottom = bottom.next
		}
		bottom.next = added
	}
	s.size++
	return nil
}

func (s *Stack) Top() (string, error) {
	if s.top == nil {
		return "", fmt.Errorf("stack is empty")
	}
	return s.top.value, nil
}

func (s *Stack) Pop() (string, error) {
	value, err := s.Top()
	if err != nil {
		return "", err
	}
	s.top = s.top.next
	s.size--
	return value, nil
}

func (s *Stack) Values() []string {
	result := make([]string, 0, s.size)
	for node := s.top; node != nil; node = node.next {
		result = append(result, node.value)
	}
	return result
}

type Queue struct {
	head *singleNode
	tail *singleNode
	size int
}

func NewQueue() *Queue                               { return &Queue{} }
func (q *Queue) Type() string                        { return "Queue" }
func (q *Queue) Size() int                           { return q.size }
func (q *Queue) Clear()                              { q.head, q.tail, q.size = nil, nil, 0 }
func (q *Queue) appendSerialized(value string) error { q.Push(value); return nil }

func (q *Queue) Push(value string) {
	added := &singleNode{value: value}
	if q.tail != nil {
		q.tail.next = added
	} else {
		q.head = added
	}
	q.tail = added
	q.size++
}

func (q *Queue) Front() (string, error) {
	if q.head == nil {
		return "", fmt.Errorf("queue is empty")
	}
	return q.head.value, nil
}

func (q *Queue) Pop() (string, error) {
	value, err := q.Front()
	if err != nil {
		return "", err
	}
	q.head = q.head.next
	q.size--
	if q.head == nil {
		q.tail = nil
	}
	return value, nil
}

func (q *Queue) Values() []string {
	result := make([]string, 0, q.size)
	for node := q.head; node != nil; node = node.next {
		result = append(result, node.value)
	}
	return result
}

type treeNode struct {
	value       string
	left, right *treeNode
}

type CompleteBinaryTree struct {
	root *treeNode
	size int
}

func NewCompleteBinaryTree() *CompleteBinaryTree                  { return &CompleteBinaryTree{} }
func (t *CompleteBinaryTree) Type() string                        { return "CompleteBinaryTree" }
func (t *CompleteBinaryTree) Size() int                           { return t.size }
func (t *CompleteBinaryTree) Clear()                              { t.root, t.size = nil, 0 }
func (t *CompleteBinaryTree) appendSerialized(value string) error { t.Insert(value); return nil }

func (t *CompleteBinaryTree) Insert(value string) {
	added := &treeNode{value: value}
	if t.root == nil {
		t.root = added
		t.size = 1
		return
	}
	pending := []*treeNode{t.root}
	for len(pending) > 0 {
		current := pending[0]
		pending = pending[1:]
		if current.left == nil {
			current.left = added
			t.size++
			return
		}
		if current.right == nil {
			current.right = added
			t.size++
			return
		}
		pending = append(pending, current.left, current.right)
	}
}

func (t *CompleteBinaryTree) Values() []string {
	result := make([]string, 0, t.size)
	if t.root == nil {
		return result
	}
	pending := []*treeNode{t.root}
	for len(pending) > 0 {
		current := pending[0]
		pending = pending[1:]
		result = append(result, current.value)
		if current.left != nil {
			pending = append(pending, current.left)
		}
		if current.right != nil {
			pending = append(pending, current.right)
		}
	}
	return result
}

func (t *CompleteBinaryTree) Contains(value string) bool {
	for _, stored := range t.Values() {
		if stored == value {
			return true
		}
	}
	return false
}

func (t *CompleteBinaryTree) IsComplete() bool {
	if t.root == nil {
		return true
	}
	pending := []*treeNode{t.root}
	gap := false
	for len(pending) > 0 {
		current := pending[0]
		pending = pending[1:]
		if current == nil {
			gap = true
			continue
		}
		if gap {
			return false
		}
		pending = append(pending, current.left, current.right)
	}
	return true
}
