use std::io::{self, Write};

/// 프롬프트를 출력하고 한 줄을 입력받는다.
fn read_line(prompt: &str) -> io::Result<String> {
    print!("{prompt}");
    io::stdout().flush()?;

    let mut input = String::new();
    io::stdin().read_line(&mut input)?;
    Ok(input.trim().to_string())
}

/// 1 이상의 정수를 입력받을 때까지 반복한다.
fn read_positive_usize(label: &str) -> usize {
    loop {
        let input = match read_line(&format!("{label}: ")) {
            Ok(line) => line,
            Err(error) => {
                eprintln!("입력 오류: {error}");
                std::process::exit(1);
            }
        };

        match input.parse::<usize>() {
            Ok(value) if value > 0 => return value,
            _ => println!("1 이상의 정수를 입력하세요."),
        }
    }
}

/// rows x cols 크기의 행렬을 한 행씩 입력받는다.
/// Rust의 Vec<Vec<i64>>로 동적 할당되며, 각 행은 정확히 cols개의 값을 가져야 한다.
fn read_matrix(name: &str, rows: usize, cols: usize) -> Vec<Vec<i64>> {
    let mut matrix = vec![vec![0_i64; cols]; rows];

    for row in 0..rows {
        loop {
            let prompt = format!("{name}의 {}/{}행 ({}개 값): ", row + 1, rows, cols);
            let input = match read_line(&prompt) {
                Ok(line) => line,
                Err(error) => {
                    eprintln!("입력 오류: {error}");
                    std::process::exit(1);
                }
            };

            let parsed: Result<Vec<i64>, _> = input
                .split_whitespace()
                .map(str::parse::<i64>)
                .collect();

            match parsed {
                Ok(values) if values.len() == cols => {
                    matrix[row] = values;
                    break;
                }
                Ok(values) => {
                    println!(
                        "값 {}개가 필요합니다. (현재 {}개 입력됨) 다시 입력하세요.",
                        cols,
                        values.len()
                    );
                }
                Err(_) => println!("정수를 공백으로 구분해 입력하세요."),
            }
        }
    }

    matrix
}

fn add_matrices(a: &[Vec<i64>], b: &[Vec<i64>]) -> Vec<Vec<i64>> {
    let rows = a.len();
    let cols = a[0].len();
    let mut result = vec![vec![0_i64; cols]; rows];

    for row in 0..rows {
        for col in 0..cols {
            result[row][col] = a[row][col] + b[row][col];
        }
    }

    result
}

fn print_matrix(name: &str, matrix: &[Vec<i64>]) {
    println!("{name} =");
    for row in matrix {
        for (index, value) in row.iter().enumerate() {
            if index > 0 {
                print!(" ");
            }
            print!("{value:>6}");
        }
        println!();
    }
}

fn main() {
    println!("== 두 행렬의 덧셈 (Rust Vec<T>) ==");

    let rows = read_positive_usize("행의 개수");
    let cols = read_positive_usize("열의 개수");

    let a = read_matrix("행렬 A", rows, cols);
    let b = read_matrix("행렬 B", rows, cols);
    let sum = add_matrices(&a, &b);

    println!();
    print_matrix("A", &a);
    println!();
    print_matrix("B", &b);
    println!();
    print_matrix("A + B", &sum);
}
