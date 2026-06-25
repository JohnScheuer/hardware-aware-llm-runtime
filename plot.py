import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("build/batch_curve.csv")

plt.figure(figsize=(8,5))
plt.plot(df["batch"], df["model_tok"], label="Model", linewidth=2)

real_df = df[df["real_tok"].notna()]
plt.scatter(real_df["batch"], real_df["real_tok"], label="Measured", s=60)

plt.xlabel("Batch Size")
plt.ylabel("Tokens / sec")
plt.title("Batch Throughput vs Model (RTX 2070)")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.savefig("docs/batch_curve.png", dpi=200)
plt.show()